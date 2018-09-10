from django.shortcuts import render, get_object_or_404, redirect
from django.http import HttpResponse
import requests
from bs4 import BeautifulSoup
import jieba
import time

from .models import News, Word

# Create your views here.

jieba.initialize()


def view_news(request, id):
    context = {
        'news': get_object_or_404(News, id=id)
    }
    return render(request, 'news/view_news.html', context)


headers = {
    'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.9 Safari/537.36'
}

def scrape_url(url):
    start_time = time.time()
    r = requests.get(url, headers=headers)
    print('request', time.time() - start_time)

    soup = BeautifulSoup(r.text, 'html.parser')
    print('parse', time.time() - start_time)

    if not soup.find('div', class_='LEFT'):
        return None 

    title = soup.find('div', class_='LEFT').h1.get_text()
    content_article = soup.find('div', class_='content-article')

    full_body = str(content_article)

    # remove embedded videos
    [x.extract() for x in content_article.findAll('script')]
    orig_content_text = ''.join(content_article.strings)
    # remove image desc
    [x.extract() for x in content_article.findAll('i', class_='desc')]

    content_text = ''.join(content_article.strings)
    abstract = content_text[:200]
    try:
        news = News.objects.get(url=url)
        news.delete()
    except News.DoesNotExist:
        pass
    news = News(url=url, title=title,
                abstract=abstract, full_body=full_body)
    news.save()

    words = list(jieba.cut_for_search(orig_content_text))
    words.extend(jieba.cut_for_search(title))
    print('jieba', time.time() - start_time)
    word_objs = []
    for word in set(words):
        try:
            word_obj = Word.objects.get(word=word)
            word_objs.append(word_obj)
        except Word.DoesNotExist:
            word_obj = Word(word=word)
            word_obj.save()
            word_objs.append(word_obj)
    news.word_set.add(*word_objs)
    print('db', time.time() - start_time)

    return news.id

def scrape(request):
    if request.method == 'GET':
        return render(request, 'news/scrape.html')
    elif request.method == 'POST':
        url = request.POST['url']
        result = scrape_url(url)
        if result:
            return redirect('view_news', id=result)
        else:
            return redirect('scrape')


def search(request):
    start_time = time.time()

    context = {}
    if 'keyword' in request.GET:
        keyword = request.GET['keyword']
        context['keyword'] = keyword
        words = jieba.cut_for_search(keyword)
        count = dict()
        for word in words:
            try:
                if word.strip() != '':
                    word_obj = Word.objects.get(word=word)
                    for news in word_obj.appearance.all():
                        if news.id in count:
                            count[news.id] = count[news.id] + 1
                        else:
                            count[news.id] = 1
            except Word.DoesNotExist:
                pass
        sorted_count = sorted(count.items(), key=lambda kv: -kv[1])
        news = []
        for (id, _) in sorted_count:
            news_obj = News.objects.get(id=id)
            news.append(news_obj)
        context['news'] = news
    else:
        context['keyword'] = ''

    context['time'] = time.time() - start_time
    return render(request, 'news/search.html', context)


def show_all(request):
    start_time = time.time()

    from_index = 0
    page = 10

    if 'from' in request.GET:
        from_index = int(request.GET['from'])
    if 'page' in request.GET:
        page = int(request.GET['page'])

    to_index = from_index + page
    prev_from_index = 0
    if from_index > page:
        prev_from_index = from_index - page

    next_from_index = from_index + page
    if to_index >= News.objects.all().count():
        # no more entries
        to_index = News.objects.all().count()
        next_from_index = from_index

    context = {
        'from_index': from_index,
        'to_index': to_index,
        'page': page,
        'prev_from_index': prev_from_index,
        'next_from_index': next_from_index,
        'news': News.objects.all()[from_index:to_index],
        'time': time.time() - start_time
    }
    return render(request, 'news/show_all.html', context)
