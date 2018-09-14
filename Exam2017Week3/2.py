import os
from html.parser import HTMLParser

score = {}


class Parser1(HTMLParser):
    begin = False
    beginTr = False
    column = 0
    rank = 0
    readData = False

    def handle_starttag(self, tag, attrs):
        if tag == 'tr':
            self.beginTr = True
            self.column = 0
        if tag == 'span' and self.beginTr:
            for key, value in attrs:
                if key == 'class' and value == 's1':
                    self.column += 1
                    self.readData = True
                    self.begin = True
        pass

    def handle_endtag(self, tag):
        if tag == 'tr':
            self.beginTr = False
        self.begin = False
        pass

    def handle_data(self, data):
        if self.begin and self.readData:
            if self.column == 1 and data.strip() != 'Rank':
                self.rank = int(data.strip())
            if self.column == 2 and data.strip() != 'Institution':
                if data.strip() not in score:
                    score[data.strip()] = 0
                score[data.strip()] += self.rank
        self.readData = False
        pass


class Parser2(HTMLParser):
    rank = 0
    readData = False

    def handle_starttag(self, tag, attrs):
        if tag == 'b':
            self.readData = True
        pass

    def handle_endtag(self, tag):
        pass

    def handle_data(self, data):
        if self.readData:
            if self.rank:
                if data.strip() not in score:
                    score[data.strip()] = 0
                score[data.strip()] += self.rank
                self.rank = 0
            elif data.strip()[0] == '#':
                self.rank = int(data.strip()[1:])
        self.readData = False
        pass


class Parser3(HTMLParser):
    beginTr = False
    rank = 0
    readData = False
    column = 0

    def handle_starttag(self, tag, attrs):
        if tag == 'tr':
            for key, value in attrs:
                if value == 'bgfd' or value == 'bgf5':
                    self.beginTr = True
                    self.column = 0
        if self.beginTr and (tag == 'td' or tag == 'a'):
            self.readData = True
        pass

    def handle_endtag(self, tag):
        if tag == 'tr':
            self.beginTr = False
        pass

    def handle_data(self, data):
        if self.readData:
            if self.rank and self.column == 1:
                if data.strip() not in score:
                    score[data.strip()] = 0
                score[data.strip()] += self.rank
                self.rank = 0
                self.column = 2
            elif self.column == 0:
                num = data.strip()
                if num.find('-') != -1:
                    from_num = int(num[:data.find('-')])
                    to_num = int(num[data.find('-') + 1:])
                    self.rank = (from_num + to_num) / 2
                else:
                    self.rank = int(data.strip())
                self.column = 1

        self.readData = False
        pass


class Parser4(HTMLParser):
    beginRank = False
    beginSchool = False
    rank = 0

    def handle_starttag(self, tag, attrs):
        if tag == 'p':
            for key, value in attrs:
                if value == 'p33' or value == 'p37':
                    self.beginRank = True
                elif value == 'p34':
                    self.beginSchool = True
        pass

    def handle_endtag(self, tag):
        if tag == 'p':
            self.beginRank = False
            self.beginSchool = False
        pass

    def handle_data(self, data):
        if self.beginRank:
            num = data.strip()
            if num.find('-') != -1:
                from_num = int(num[:data.find('-')])
                to_num = int(num[data.find('-') + 1:])
                self.rank = (from_num + to_num) / 2
            elif num.find('=') != -1:
                self.rank = int(num[1:])
            else:
                self.rank = int(num)
        elif self.beginSchool and self.rank:
            if data.strip() not in score:
                score[data.strip()] = 0
            score[data.strip()] += self.rank
            self.rank = 0
        pass


with open('./2/csrank1.html', 'r') as fd:
    parser = Parser1()
    parser.feed(fd.read())
with open('./2/csrank2.html', 'r') as fd:
    parser = Parser2()
    parser.feed(fd.read())
with open('./2/csrank3.html', 'r') as fd:
    parser = Parser3()
    parser.feed(fd.read())
with open('./2/csrank4.html', 'r') as fd:
    parser = Parser4()
    parser.feed(fd.read())

data = sorted(score.items(), key=lambda kv: kv[1])
with open('ranklist.txt', 'w') as fd:
    i = 1
    for school, score in data:
        fd.write(f'{i}: {school}\n')
        i += 1
print(data)
