import os
os.environ.setdefault("DJANGO_SETTINGS_MODULE", "Week3.settings")
import django
django.setup()

from news import views
import sys
import requests
from bs4 import BeautifulSoup
import time
import random
from news.models import News

for kind in ['ent', 'sports', 'house', 'tech', 'auto', 'digi', 'fashion', 'astro', 'gamezone', 'cul', 'edu']:
    try:
        common_ua = [
            'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/601.7.7 (KHTML, like Gecko) Version/9.1.2 Safari/601.7.7',
            'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/48.0.2564.109 Safari/537.36',
            'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
            'Mozilla/5.0 (Windows NT 10.0; WOW64; rv:56.0) Gecko/20100101 Firefox/56.0',
            'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.89 Safari/537.36']
        url = f'http://news.qq.com/ninja/timeline_{kind}tab_newsindex.htm'
        headers = {
            'User-Agent': common_ua[random.randint(0, len(common_ua)-1)],
            'DNT': '1',
            'Referer': 'http://news.qq.com',
            'Accept': '*/*',
            'Accept-Encoding': 'gzip, deflate',
            'X-Requested-With': 'XMLHttpRequest'
        }

        print(f'start to pull list from {url}')
        r = requests.get(url, headers=headers)
        soup = BeautifulSoup(r.text, 'html.parser')
        links = soup.find_all('a', class_='linkto')
        urls = [x['href'] for x in links]
        print(f'Got {len(urls)} links')
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
                    break
                except:
                    timeout = timeout * 2
                    pass
                pass
    except KeyboardInterrupt:
        break
    except:
        pass
