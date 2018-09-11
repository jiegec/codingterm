import os
os.environ.setdefault("DJANGO_SETTINGS_MODULE", "Week3.settings")
import django
django.setup()

from news import views
import sys
import requests
from bs4 import BeautifulSoup
import time

for kind in ['sports', 'finance', 'house', 'tech', 'auto', 'digi', 'fashion']:
    try:
        url = f'http://news.qq.com/ninja/timeline_{kind}tab_newsindex.htm'
        headers = {
            'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.9 Safari/537.36',
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
            timeout = 2
            try:
                print(f'scraping {url}')
                views.scrape_url(url)
                time.sleep(timeout)
                timeout = 2
            except:
                timeout = timeout * 2
                pass
    except:
        pass
