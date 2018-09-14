import os
from html.parser import HTMLParser
import datetime


result = []
title = ""


class Parser(HTMLParser):
    beginTr = False
    beginTitle = False
    row = -1

    def handle_starttag(self, tag, attrs):
        if tag == 'tr':
            self.beginTr = True
            self.row += 1
            flag = False
            for key, value in attrs:
                if key == 'bgcolor':
                    result.append([value])
                    flag = True
            if not flag:
                result.append([''])
        if tag == 'title':
            self.beginTitle = True
        pass

    def handle_endtag(self, tag):
        if tag == 'tr':
            self.beginTr = False
        if tag == 'title':
            self.beginTitle = False

    def handle_data(self, data):
        if self.beginTr:
            result[self.row].append(data.strip())
        if self.beginTitle:
            global title
            title = data.strip()


with open('List of cities in China.html', 'r') as fd:
    parser = Parser()
    parser.feed(fd.read())

cities = []
with open(f'{title}.csv', 'w') as fd:
    fd.write(','.join(filter(lambda k: len(k) > 0, result[0])) + '\n')
    for city in result[1:]:
        if city[9] == 'ZZZZ-none':
            fd.write(f'{city[2]},{city[4]},{city[7]},{city[10]},{city[12]}\n')
            cities.append([city[0], city[2], city[4],
                           city[7], city[10], city[12]])
        elif city[9] == 'direct administration' or city[8] == 'ZZZZ-none':
            fd.write(f'{city[2]},{city[4]},{city[6]},{city[9]},{city[11]}\n')
            cities.append([city[0], city[2], city[4],
                           city[6], city[9], city[11]])
        elif city[10] == 'direct administration':
            fd.write(f'{city[2]},{city[5]},{city[7]},{city[10]},{city[12]}\n')
            cities.append([city[0], city[2], city[5],
                           city[7], city[10], city[12]])
        else:
            fd.write(f'{city[2]},{city[4]},{city[6]},{city[8]},{city[10]}\n')
            cities.append([city[0], city[2], city[4],
                           city[6], city[8], city[10]])

with open(f'color.csv', 'w') as fd:
    fd.write(','.join(filter(lambda k: len(k) > 0, result[0])) + '\n')
    for city in result[1:]:
        if len(city[0].strip()) > 0:
            if city[9] == 'ZZZZ-none':
                fd.write(
                    f'{city[2]},{city[4]},{city[7]},{city[10]},{city[12]}\n')
            elif city[9] == 'direct administration' or city[8] == 'ZZZZ-none':
                fd.write(
                    f'{city[2]},{city[4]},{city[6]},{city[9]},{city[11]}\n')
            elif city[10] == 'direct administration':
                fd.write(
                    f'{city[2]},{city[5]},{city[7]},{city[10]},{city[12]}\n')
            else:
                fd.write(
                    f'{city[2]},{city[4]},{city[6]},{city[8]},{city[10]}\n')

cities = sorted(
    cities, key=lambda city: datetime.datetime.strptime(city[-1], '%Y-%m-%d'))
with open('output.html', 'w') as out:
    content = ''
    for city in cities:
        if city[3] == 'autonomous' or city[3] == 'municipal':
            content += f'''
    <tr bgcolor="{city[0]}">
    <td><a href="{city[1].replace(' ','_')}" title="{city[1]}">{city[1]}</a></td>
    <td><span style="font-size:125%;"><span lang="zh-CN" title="Chinese language text">{city[2]}</span></span></td>
    <td><span style="font-size:85%;"><i>{city[3]}</i></span></td>
    <td><span style="font-size:85%;"><i>{city[4]}</i></span></td>
    <td>{city[5]}
    </td></tr>
            '''
        elif city[4] != 'direct administration':
            content += f'''
    <tr bgcolor="{city[0]}">
    <td><a href="{city[1].replace(' ','_')}" title="{city[1]}">{city[1]}</a></td>
    <td><span style="font-size:125%;"><span lang="zh-CN" title="Chinese language text">{city[2]}</span></span></td>
    <td><a href="{city[3]}" title="{city[3]}">{city[3]}</a></td>
    <td><a href="{city[4]}" title="{city[4]}">{city[4]}</a></td>
    <td>{city[5]}
    </td></tr>
            '''
        else:
            content += f'''
    <tr bgcolor="{city[0]}">
    <td><a href="{city[1].replace(' ','_')}" title="{city[1]}">{city[1]}</a></td>
    <td><span style="font-size:125%;"><span lang="zh-CN" title="Chinese language text">{city[2]}</span></span></td>
    <td><a href="{city[3]}" title="{city[3]}">{city[3]}</a></td>
    <td><span style="font-size:85%;"><i>{city[4]}</i></span></td>
    <td>{city[5]}
    </td></tr>
            '''

    template = '''
<!DOCTYPE html>
<html class="client-nojs" lang="en" dir="ltr">
<head>
    <meta charset="UTF-8"/>
    <title>Cities in China</title>
    <style type="text/css">
        table
            {
            border-collapse:collapse;
            }

        table, td, th
            {
            border:1px solid black;
            }
    </style>
</head>

<body>
    <table class="wikitable sortable selected_now jquery-tablesorter" id="cities">

    <thead><tr>
    <th class="headerSort" tabindex="0" role="columnheader button" title="Sort ascending">City</th>
    <th class="headerSort" tabindex="0" role="columnheader button" title="Sort ascending">Chinese</th>
    <th class="headerSort" tabindex="0" role="columnheader button" title="Sort ascending">Province</th>
    <th class="headerSort" tabindex="0" role="columnheader button" title="Sort ascending">Prefecture</th>
    <th class="headerSort" tabindex="0" role="columnheader button" title="Sort ascending">Founded</th>
    </tr></thead><tbody>''' + content + '''
    </tbody><tfoot></tfoot></table>
</body>
</html>
'''
    out.write(template)
