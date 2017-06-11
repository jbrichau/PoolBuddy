const HtmlWebpackPlugin = require('html-webpack-plugin'); //installed via npm
const webpack = require('webpack'); //to access built-in plugins
const path = require('path');

module.exports = function(env) {
  return {
    entry: './src/app/index.js',
    output: {
      path: path.resolve(__dirname, 'dist'),
      filename: 'bundle.js'
    },
    module: {
      rules: [
        {
          test: /\.(js|jsx)$/,
          use: 'babel-loader'
        }
      ]
    },
    plugins: [
      new webpack.optimize.UglifyJsPlugin({
        compress: process.env.NODE_ENV == "production"
      }),
      new HtmlWebpackPlugin({template: './src/app/index.html.tpl'})
    ]
  };
}
