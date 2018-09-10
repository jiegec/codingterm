from django.urls import path

from . import views

urlpatterns = [
    path('<int:id>/', views.view_news, name='view_news'),
    path('scrape/', views.scrape, name='scrape'),
    path('search/', views.search, name='search'),
    path('all/', views.show_all, name='show_all'),
]