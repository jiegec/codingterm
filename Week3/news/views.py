from django.shortcuts import render, get_object_or_404, redirect
from django.http import HttpResponse
from django.db import transaction
from django.utils.dateparse import parse_datetime
import pytz
import requests
from bs4 import BeautifulSoup
import jieba
import time
import random
import json
import re


from .models import News, Word

# Create your views here.


def view_news(request, id):
    context = {
        'news': get_object_or_404(News, id=id)
    }
    return render(request, 'news/view_news.html', context)


def feeling_lucky(request):
    if News.objects.count() == 0:
        return Http404("No news available")
    context = {
        'news': News.objects.all()[random.randint(0, News.objects.count()-1)]
    }
    return render(request, 'news/view_news.html', context)


def scrape_url(url):
    common_ua = [
        'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/601.7.7 (KHTML, like Gecko) Version/9.1.2 Safari/601.7.7',
        'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/48.0.2564.109 Safari/537.36',
        'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
        'Mozilla/5.0 (Windows NT 10.0; WOW64; rv:56.0) Gecko/20100101 Firefox/56.0',
        'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.89 Safari/537.36']
    headers = {
        'User-Agent': common_ua[random.randint(0, len(common_ua)-1)],
        'DNT': '1',
        'Referer': 'http://news.qq.com',
        'Accept': '*/*',
        'Accept-Encoding': 'gzip, deflate',
    }

    start_time = time.time()
    print('start to scrape')
    r = requests.get(url, headers=headers)
    print('request', time.time() - start_time)

    soup = BeautifulSoup(r.text, 'html.parser')
    print('parse', time.time() - start_time)

    if soup.find('div', class_='LEFT'):
        title = soup.find('div', class_='LEFT').h1.get_text()
        content_article = soup.find('div', class_='content-article')
        if content_article.find('div', class_='PictureWrap'):
            script_content = content_article.find('script').text
            json_content = script_content[len('IMGDATA = '):]
            data = json.loads(json_content)
            for row in data:
                if row['type'] == 2:
                    # image
                    txt = soup.new_tag('p')
                    txt['class'] = 'one-p'
                    img = soup.new_tag('img', src=row['value'])
                    img['class'] = 'content-picture'
                    txt.append(img)
                    content_article.append(txt)
                elif row['type'] == 1:
                    # text
                    txt = soup.new_tag('p')
                    txt['class'] = 'one-p'
                    txt.string = row['value']
                    content_article.append(txt)
    elif soup.find('div', class_='qq_article'):
        title = soup.find('div', class_='hd').h1.get_text()
        content_article = soup.find('div', class_='Cnt-Main-Article-QQ')
    else:
        return None

    pub_date = None
    if soup.find('meta', {'name': '_pubtime'}):
        date = parse_datetime(soup.find('meta', {'name': '_pubtime'})['content'])
        pub_date = pytz.timezone('Asia/Shanghai').localize(date)

    full_body = str(content_article)

    # remove embedded videos
    [x.extract() for x in content_article.find_all('script')]
    [x.extract() for x in content_article.find_all('style')]
    [x.extract() for x in content_article.find_all('div', class_='rv-js-root')]
    orig_content_text = ''.join(content_article.strings)
    # remove image desc
    [x.extract() for x in content_article.find_all('i', class_='desc')]

    content_text = ''.join(content_article.strings)
    abstract = content_text[:200].strip()
    try:
        news = News.objects.get(url=url)
        news.word_set.clear()
        news.url = url
        news.title = title
        news.abstract = abstract
        news.full_body = full_body
        news.pub_date = pub_date
    except News.DoesNotExist:
        news = News(url=url, title=title,
                    abstract=abstract, full_body=full_body, pub_date=pub_date)
        pass
    news.save()

    words = list(jieba.cut_for_search(orig_content_text))
    words.extend(jieba.cut_for_search(title))
    print('jieba', time.time() - start_time)

    word_objs = []
    not_exists = []
    for word in set(words):
        if word.strip() != '':
            try:
                word_obj = Word.objects.get(word=word)
                word_objs.append(word_obj)
            except Word.DoesNotExist:
                not_exists.append(word)
    for word in not_exists:
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


def paging(params, context, count):
    from_index = 0
    page = 10

    if 'from' in params:
        from_index = int(params['from'])
    if 'page' in params:
        page = int(params['page'])

    to_index = from_index + page
    prev_from_index = 0
    if from_index > page:
        prev_from_index = from_index - page

    next_from_index = from_index + page
    if to_index >= count:
        # no more entries
        to_index = count
        next_from_index = from_index

    context['from_index'] = from_index
    context['to_index'] = to_index
    context['page'] = page
    context['prev_from_index'] = prev_from_index
    context['next_from_index'] = next_from_index


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
                        number = sum(1 for _ in re.finditer(
                            re.escape(word), news.full_body))
                        number = number + \
                            sum(1 for _ in re.finditer(
                                re.escape(word), news.title)) * 10
                        if news.id in count:
                            count[news.id] = count[news.id] + number
                        else:
                            count[news.id] = number
            except Word.DoesNotExist:
                pass
        sorted_count = sorted(count.items(), key=lambda kv: -kv[1])
        news = []
        for (id, count) in sorted_count:
            news_obj = News.objects.get(id=id)
            news.append(news_obj)
            # print(f'{id}: {count}')

        paging(request.GET, context, len(news))
        context['news'] = news[context['from_index']:context['to_index']]
        context['time'] = time.time() - start_time
    else:
        context['keyword'] = ''

    return render(request, 'news/search.html', context)


def show_all(request):
    start_time = time.time()

    context = {}
    paging(request.GET, context, News.objects.count())
    context['news'] = News.objects.all()[context['from_index']:context['to_index']]
    context['time'] = time.time() - start_time
    return render(request, 'news/show_all.html', context)
