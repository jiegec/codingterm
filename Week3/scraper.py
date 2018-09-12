import os
os.environ.setdefault("DJANGO_SETTINGS_MODULE", "Week3.settings")
import django
django.setup()

from news import views
import sys
from bs4 import BeautifulSoup
import time
import random
from news.models import News
import json
import requests

''' 
import requests
import logging

# Enabling debugging at http.client level (requests->urllib3->http.client)
# you will see the REQUEST, including HEADERS and DATA, and RESPONSE with HEADERS but without DATA.
# the only thing missing will be the response.body which is not logged.
try:  # for Python 3
    from http.client import HTTPConnection
except ImportError:
    from httplib import HTTPConnection
HTTPConnection.debuglevel = 1

# you need to initialize logging, otherwise you will not see anything from requests
logging.basicConfig()
logging.getLogger().setLevel(logging.DEBUG)
requests_log = logging.getLogger("urllib3")
requests_log.setLevel(logging.DEBUG)
requests_log.propagate = True
'''

s = requests.Session()
common_ua = [
    'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/601.7.7 (KHTML, like Gecko) Version/9.1.2 Safari/601.7.7',
    'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/48.0.2564.109 Safari/537.36',
    'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
    'Mozilla/5.0 (Windows NT 10.0; WOW64; rv:56.0) Gecko/20100101 Firefox/56.0',
    'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.89 Safari/537.36']

def go_scrape_links(urls):
    for url in urls:
        try:
            News.objects.get(url=url)
            print(f'Already scraped {url}, ignoring')
            # already scraped
        except News.DoesNotExist:
            timeout = 2
            try:
                views.scrape_url(url)
                time.sleep(timeout)
                timeout = 2
            except KeyboardInterrupt:
                raise
            except:
                timeout = timeout * 2
                pass
            pass

if True:
    try:
        url = f'http://news.qq.com/'
        headers = {
            'User-Agent': common_ua[random.randint(0, len(common_ua)-1)],
            'DNT': '1',
            'Referer': 'http://news.qq.com',
            'Accept': '*/*',
            'Accept-Encoding': 'gzip, deflate',
            'Accept-Language': 'zh-CH,zh;q=0.9,en;q=0.8',
            'X-Requested-With': 'XMLHttpRequest'
        }

        print(f'start to pull list from {url}')
        r = s.get(url, headers=headers)
        soup = BeautifulSoup(r.text, 'html.parser')
        links = soup.find_all('a', class_='pic')
        urls = [x['href'] for x in links]
        print(f'Got {len(urls)} links')
        go_scrape_links(urls)
    except:
        pass

if True:
    for site in ['news', 'ent', 'sports', 'finance', 'tech', 'games', 'auto', 'edu', 'house']:
        try:
            url = f'http://roll.news.qq.com/interface/cpcroll.php?callback=rollback&site={site}&mode=1&cata=&date={time.strftime("%Y-%m-%d")}&page=1&_={int(time.time()*1000)}'
            headers = {
                'User-Agent': common_ua[random.randint(0, len(common_ua)-1)],
                'DNT': '1',
                'Referer': 'http://news.qq.com/articleList/rolls/',
                'Accept': '*/*',
                'Accept-Encoding': 'gzip, deflate',
                'Accept-Language': 'zh-CH,zh;q=0.9,en;q=0.8',
                'X-Requested-With': 'XMLHttpRequest'
            }

            print(f'start to pull list from {url}')
            r = s.get(url, headers=headers)
            data = json.loads(r.text[len('rollback('):-1])
            urls = [x['url'] for x in data['data']['article_info']]
            print(f'Got {len(urls)} links')
            go_scrape_links(urls)
        except KeyboardInterrupt:
            raise
        except:
            pass

if True:
    for kind in ['ent', 'sports', 'house', 'tech', 'auto', 'digi', 'fashion', 'astro', 'gamezone', 'cul', 'edu']:
        try:
            url = f'http://news.qq.com/ninja/timeline_{kind}tab_newsindex.htm'
            headers = {
                'User-Agent': common_ua[random.randint(0, len(common_ua)-1)],
                'DNT': '1',
                'Referer': 'http://news.qq.com',
                'Accept': '*/*',
                'Accept-Encoding': 'gzip, deflate',
                'Accept-Language': 'zh-CH,zh;q=0.9,en;q=0.8',
                'X-Requested-With': 'XMLHttpRequest'
            }

            print(f'start to pull list from {url}')
            r = s.get(url, headers=headers)
            soup = BeautifulSoup(r.text, 'html.parser')
            links = soup.find_all('a', class_='linkto')
            urls = [x['href'] for x in links]
            print(f'Got {len(urls)} links')
            go_scrape_links(urls)
        except KeyboardInterrupt:
            raise
        except:
            pass
