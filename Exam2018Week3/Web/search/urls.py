from django.urls import path

from . import views

urlpatterns = [
    path('search_name/', views.search_name, name='search_name'),
    path('search_date/', views.search_date, name='search_date'),
    path('city/', views.city, name='city'),
]
