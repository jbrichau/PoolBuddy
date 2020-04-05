import express from 'express';
import path from 'path';
import { Datastore } from '@google-cloud/datastore';
import config from './config';

const isDevelopment = process.env.NODE_ENV !== "production";
const app = express();
const dataStoreConfig = { "projectId": config.projectId };

if(isDevelopment)
  dataStoreConfig.keyFilename = 'config/poolbuddy-apikey.json';
else
  require('@google-cloud/debug-agent').start();

app.post('/pooldata', (request, response) => {
  const datastore = new Datastore(dataStoreConfig);
  const yesterday = new Date();
  yesterday.setDate(yesterday.getDate() - 2);
  const query = datastore.createQuery('pooldata')
    .filter('timestamp', '>=', yesterday)
    .order('timestamp', {
      descending: false
    });
  datastore.runQuery(query, function(err, entities, info) {
    response.json([err,info,entities]);
  });
});

app.post('/soildata', (request, response) => {
  const datastore = new Datastore(dataStoreConfig);
  const yesterday = new Date();
  yesterday.setDate(yesterday.getDate() - 2);
  const query = datastore.createQuery('soildata')
    .filter('timestamp', '>=', yesterday)
    .order('timestamp', {
      descending: false
    });
  datastore.runQuery(query, function(err, entities, info) {
    response.json([err,info,entities]);
  });
});

app.post('/weatherdata', (request, response) => {
  const datastore = new Datastore(dataStoreConfig);
  const yesterday = new Date();
  yesterday.setDate(yesterday.getDate() - 2);
  const query = datastore.createQuery('weatherdata')
    .filter('timestamp', '>=', yesterday)
    .order('timestamp', {
      descending: false
    });
  datastore.runQuery(query, function(err, entities, info) {
    response.json([err,info,entities]);
  });
});

if(isDevelopment) {
  const webpackDevMiddleware = require("webpack-dev-middleware");
  const webpack = require("webpack");
  const webpackConfig = require("../../webpack.config");
  webpackConfig.mode = 'development';
  const compiler = webpack(webpackConfig);
  app.use(webpackDevMiddleware(compiler, {
    publicPath: "/" // Same as `output.publicPath` in most cases.
  }));
} else {
  app.use(express.static(path.join(__dirname, '../dist')));
}

app.listen(config.port, () => {
  console.log('Server started on port ' + config.port);
});
