from django.shortcuts import render, redirect
from django.http import HttpResponse
from .models import User

# Create your views here.


def register(request):
    context = {}
    if request.method == 'GET':
        return render(request, 'register/register.html', context)
    else:
        if not 'username' in request.POST:
            context['error'] = 'No user name'
            return render(request, 'register/register.html', context)
        if not 'password' in request.POST or not 'password-repeat' in request.POST or request.POST['password'] != request.POST['password-repeat']:
            context['error'] = 'Bad password'
            return render(request, 'register/register.html', context)
        if not 'kind' in request.POST:
            context['error'] = 'Bad user kind'
            return render(request, 'register/register.html', context)
        try:
            user = User.objects.get(name=request.POST['username'])
            context['error'] = 'User already exists'
            return render(request, 'register/register.html', context)
        except User.DoesNotExist:
            user = User(name=request.POST['username'], password=request.POST['password'],
                kind=request.POST['kind'])
            user.save()
            return redirect('login')


def login(request):
    if request.session.get('user'):
        return redirect('home')
    context = {}
    if request.method == 'GET':
        return render(request, 'register/login.html', context)
    else:
        if not 'username' in request.POST:
            context['error'] = 'No user name'
            return render(request, 'register/login.html', context)
        if not 'password' in request.POST:
            context['error'] = 'Bad password'
            return render(request, 'register/login.html', context)
        
        try:
            user = User.objects.get(name=request.POST['username'])
            if user.password != request.POST['password']:
                context['error'] = 'Wrong password'
                return render(request, 'register/login.html', context)
            request.session['user'] = user.name
            return redirect('home')
        except User.DoesNotExist:
            context['error'] = 'User does not exist'
            return render(request, 'register/login.html', context)

def home(request):
    context = {}
    if not request.session.get('user'):
        return redirect('login')
    context['user'] = User.objects.get(name=request.session.get('user'))
    context['all_users'] = User.objects.all()
    return render(request, 'register/home.html', context)

def logout(request):
    if request.session.get('user'):
        del request.session['user']
    return redirect('login')

def delete_user(request):
    if request.session.get('user') and 'name' in request.POST:
        user = User.objects.get(name=request.session.get('user'))
        if user.kind == 'admin':
            try:
                user_to_delete = User.objects.get(name=request.POST['name'])
                user_to_delete.delete()
            except User.DoesNotExist:
                pass

    return redirect('home')
