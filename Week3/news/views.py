from django.shortcuts import render, get_object_or_404, redirect
from django.http import HttpResponse
from django.db import transaction, connection
from django.utils.dateparse import parse_datetime
from django.db.models import Count, Q
import pytz
import requests
from bs4 import BeautifulSoup
import jieba
from jieba.analyse import extract_tags
import time
import random
import json
import re
import math

from .models import News, Word

# Create your views here.


def view_news(request, id):
    start_time = time.time()
    show_news = get_object_or_404(News, id=id)
    context = {}
    context['news'] = show_news
    context['time'] = time.time() - start_time
    return render(request, 'news/view_news.html', context)


def similar_news(request, id):
    start_time = time.time()
    show_news = get_object_or_404(News, id=id)
    context = {}
    news_count = News.objects.count()
    threshold = math.ceil(news_count / math.exp(4.0))  # idf threshold
    word_ids = set(show_news.word_set.values_list('id', flat=True))
    # find large idf words
    # high_appearance_words = Word.objects.annotate(
    # news_count=Count('appearance')).filter(appearance=show_news, news_count__lte=threshold, news_count__gte=2).values_list('id', flat=True)
    #high_appearance_words = list(high_appearance_words)
    content_article = BeautifulSoup(show_news.full_body, 'html.parser')
    [x.extract() for x in content_article.find_all('script')]
    [x.extract() for x in content_article.find_all('style')]
    [x.extract() for x in content_article.find_all('div', class_='rv-js-root')]
    orig_content_text = ''.join(content_article.strings)
    # remove image desc
    [x.extract() for x in content_article.find_all('i', class_='desc')]

    content_text = ''.join(content_article.strings)
    tags = extract_tags(f'{show_news.title} {content_text}', topK=10)
    high_appearance_words = tags
    weight = {}
    # Only one db query
    query = Q()
    for word in high_appearance_words:
        count = Word.objects.get(word=word).appearance.count()
        if count <= threshold:
            query |= Q(word__word=word)
            print(word)
    query = News.objects.filter(query)
    news_ids = query.values_list('id', flat=True)
    #related_news = News.objects.in_bulk(list(news_ids))
    #related_news = list(related_news.values())
    print('before', time.time() - start_time)
    print(len(news_ids))
    for news in news_ids:
        if news != show_news.id:
            intersect = Word.objects.filter(Q(appearance=show_news) | Q(
                appearance=news)).annotate(count=Count('id')).filter(count=2).count()
            union = Word.objects.filter(
                Q(appearance=show_news) | Q(appearance=news)).count()
            #intersect = Word.objects.filter(Q(appearance=show_news) & Q(appearance=news)).count()
            #union = Word.objects.filter(Q(appearance=show_news) | Q(appearance=news)).count()
            #current_word_ids = set(Word.objects.filter(appearance=news).values_list('id', flat=True))
            # weight[news] = float(
            # len(word_ids & current_word_ids)) / len(word_ids | current_word_ids)
            weight[news] = float(intersect) / union

    print('after', time.time() - start_time)
    sorted_weight = sorted(weight.items(), key=lambda kv: kv[1], reverse=True)
    similar_news = []
    print('best news:')
    for (id, weight) in sorted_weight[:3]:
        news_obj = News.objects.get(id=id)
        similar_news.append(news_obj)
        print(f'{news_obj.title}: {weight}')
    context['similar_news'] = similar_news
    print('result', time.time() - start_time)
    return render(request, 'news/similar_news.html', context)


def feeling_lucky(request):
    if News.objects.count() == 0:
        return Http404("No news available")
    context = {
        'news': News.objects.all()[random.randint(0, News.objects.count()-1)]
    }
    return render(request, 'news/view_news.html', context)


s = requests.Session()


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
    print(f'scraping {url}')
    r = s.get(url, headers=headers)
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
                    if 'desc' in row:
                        txt.append(row['desc'])
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
    pubtime = re.search(
        r'pubtime:\'([0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2})\'', r.text)
    if soup.find('meta', {'name': '_pubtime'}):
        date = parse_datetime(
            soup.find('meta', {'name': '_pubtime'})['content'])
        pub_date = pytz.timezone('Asia/Shanghai').localize(date)
    elif pubtime:
        date = parse_datetime(f'{pubtime.group(1)}:00')
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
    # FIXME: use get_or_insert()?
    try:
        news = News.objects.get(url=url)
        # FIXME: Don't clear, and use word_set.set() instead?
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
    # FIXME: use exists?
    for word in set(words):
        if word.strip() != '':
            try:
                word_obj = Word.objects.get(word=word)
                word_objs.append(word_obj)
            except Word.DoesNotExist:
                not_exists.append(word)
    # FIXME: use bulk_insert for better performance
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
        from_time = None
        to_time = None
        if 'from_time' in request.GET:
            from_time = parse_datetime(request.GET['from_time'])
        if 'to_time' in request.GET:
            to_time = parse_datetime(request.GET['to_time'])
        keyword = request.GET['keyword']
        context['keyword'] = keyword
        words = set(jieba.cut_for_search(keyword))
        count = dict()
        news_count = News.objects.count()
        for word in words:
            try:
                if word.strip() != '':
                    word_obj = Word.objects.get(word=word)
                    num_appearance = word_obj.appearance.count()
                    if num_appearance == 0:
                        # no appearance
                        pass
                    idf = math.log(float(news_count) / num_appearance)
                    if idf < 0.5:
                        # handle stop words
                        idf = 0.01
                    regexp = re.compile(re.escape(word))
                    all_news = word_obj.appearance.all()
                    if from_time:
                        context['from_time'] = from_time
                        all_news = all_news.filter(pub_date__gte=pytz.timezone(
                            'Asia/Shanghai').localize(from_time))
                    if to_time:
                        context['to_time'] = to_time
                        all_news = all_news.filter(pub_date__lte=pytz.timezone(
                            'Asia/Shanghai').localize(to_time))
                    for news in all_news:
                        tf = math.log(
                            1 + sum(1 for _ in regexp.finditer(news.full_body)))
                        tf = tf + \
                            math.log(
                                1+sum(1 for _ in regexp.finditer(news.title)) * 10)
                        coef = tf * idf
                        if news.id in count:
                            count[news.id] = count[news.id] + coef
                        else:
                            count[news.id] = coef
            except Word.DoesNotExist:
                pass
        sorted_count = sorted(count.items(), key=lambda kv: -kv[1])
        news = []
        paging(request.GET, context, len(sorted_count))
        for (id, count) in sorted_count[context['from_index']:context['to_index']]:
            news_obj = News.objects.get(id=id)
            for word in words:
                news_obj.title = str(news_obj.title).replace(
                    word, f'<em>{word}</em>')
                news_obj.abstract = str(news_obj.abstract).replace(
                    word, f'<em>{word}</em>')
            news.append(news_obj)
            # print(f'{id}: {count}')

        context['news'] = news
        if 'from_time' in request.GET:
            context['from_time_text'] = request.GET['from_time']
        if 'to_time' in request.GET:
            context['to_time_text'] = request.GET['to_time']
        context['time'] = time.time() - start_time
    else:
        context['keyword'] = ''

    return render(request, 'news/search.html', context)


def show_all(request):
    start_time = time.time()

    context = {}
    paging(request.GET, context, News.objects.count())
    context['news'] = News.objects.all()[context['from_index']                                         :context['to_index']]
    context['time'] = time.time() - start_time
    return render(request, 'news/show_all.html', context)
