
// module.exports = Object.assign(
//   {},
//   require('./device'),
//   require('./scene'),
//   require('./io'),
//   require('./notification'),
//   require('./mode'),
// )

const context = require.context('./', false, /\.js$/);
const keys = context.keys().filter(item => item !== './index.js');

const re = {};

for (let i = 0; i < keys.length; i ++) {
  Object.assign(re, context(keys[i]));
}

module.exports = re;
