const io = require('socket.io-client');
const { notification } = require('antd');
const actions = require('../actions');


module.exports = function (url, store) {
  const socket = io.connect(url);
  let errKey;
  store.dispatch(actions['io/connect']());
  // 监听socket事件
  socket
    .on('connect', () => {
      if (errKey) {
        notification.close(errKey);
        notification.success({
          message: '消息服务已恢复'
        });
      }
      store.dispatch(actions['io/connect.success']())
    })
    .on('error', err => {
      errKey = (new Date()).valueOf();
      notification.error({
        message: '消息服务错误',
        description: '连接已丢失',
        key: errKey,
        duration: null
      });
      store.dispatch(actions['io/connect.failure'](err))
    })
    .on('reconnect', () => {
      store.dispatch(actions['io/reconnect.success']())
    })
    .on('data', ({type, payload}) => {
      store.dispatch(actions['io/receive.success'](type, payload))
    })
};
