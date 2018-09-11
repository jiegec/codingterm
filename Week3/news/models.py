from django.db import models

# Create your models here.


class News(models.Model):
    url = models.CharField(max_length=100, unique=True)
    title = models.CharField(max_length=100)
    abstract = models.CharField(max_length=500)
    pub_date = models.DateTimeField(null=True)
    full_body = models.TextField()

    def __str__(self):
        return f"<News: url:{self.url} title:{self.title}>"


class Word(models.Model):
    word = models.CharField(max_length=40, unique=True)
    appearance = models.ManyToManyField(News)

    def __str__(self):
        return f"<Word: {self.word}>"