const webpack = require("webpack");
const merge = require("webpack-merge");
const TerserPlugin = require('terser-webpack-plugin');
const CompressionPlugin = require('compression-webpack-plugin');
const common = require("./webpack.config");

module.exports = merge(common, {
  mode: "production",
  devtool: "source-map",
  optimization: {
    minimizer: [ 
      new TerserPlugin({
        'sourceMap': true,
        'terserOptions': {
          'keep_fnames': true
        }
      }) ] },
  plugins: [
    new webpack.DefinePlugin({
      "process.env.NODE_ENV": JSON.stringify("production"),
    }),
    new CompressionPlugin()
  ],
});
