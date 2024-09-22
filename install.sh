#!/bin/sh
sudo apt-get install make
sudo apt-get install g++
sudo apt-get install postgresql
sudo apt-get install redis
sudo apt-get install libhiredis-dev
sudo apt-get install libpq-dev
sudo apt-get install libpistache-dev
sudo apt-get install curl
sudo apt-get install nlohmann-json3-dev

sudo usermod -aG $USER postgres
