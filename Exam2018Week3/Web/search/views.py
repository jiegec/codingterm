from django.shortcuts import render
import csv
from django.shortcuts import render, redirect
from django.http import HttpResponse
from django.utils.dateparse import parse_date


cities = []

with open('cities.csv', 'r') as csvfile:
    cities = list(csv.reader(csvfile, delimiter=','))

# Create your views here.


def search_name(request):
    context = {}
    context['cities'] = []
    if 'province' in request.GET:
        context['province'] = request.GET['province']
    else:
        context['province'] = ''
    if 'province' in request.GET:
        for city in cities[1:]:
            if city[2] == request.GET['province']:
                context['cities'].append({
                    'name': city[0],
                    'chinese': city[1],
                    'province': city[2],
                    'perfecture': city[3],
                    'founded': city[4],
                })
    context['cities_len'] = len(context['cities'])
    return render(request, 'search/search_name.html', context)


def search_date(request):
    context = {}
    context['cities'] = []
    from_date = None
    if 'from_date' in request.GET:
        context['from_date'] = request.GET['from_date']
        from_date = parse_date(request.GET['from_date'])
    else:
        context['from_date'] = ''
    to_date = None
    if 'to_date' in request.GET:
        context['to_date'] = request.GET['to_date']
        to_date = parse_date(request.GET['to_date'])
    else:
        context['to_date'] = ''
    for city in cities[1:]:
        date = parse_date(city[4])
        if from_date and date < from_date:
            continue
        if to_date and date > to_date:
            continue
        context['cities'].append({
            'name': city[0],
            'chinese': city[1],
            'province': city[2],
            'perfecture': city[3],
            'founded': city[4],
        })
    context['cities_len'] = len(context['cities'])
    return render(request, 'search/search_date.html', context)


def city(request):
    context = {}
    for city in cities[1:]:
        if city[0] == request.GET['city']:
            context['city'] = {
                'name': city[0],
                'chinese': city[1],
                'province': city[2],
                'perfecture': city[3],
                'founded': city[4],
            }

    context['children'] = []
    for city in cities[1:]:
        if city[3] == request.GET['city']:
            context['children'].append({
                'name': city[0],
                'chinese': city[1],
                'province': city[2],
                'perfecture': city[3],
                'founded': city[4],
            })
    return render(request, 'search/city.html', context)
