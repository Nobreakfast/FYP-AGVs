#!/bin/sh

git config --global user.email "zcl.neber@hotmail.com"
git config --global user.name "nobreakfast"
git add .
git commit -am "$1"
git pull origin master
git push -u origin master
