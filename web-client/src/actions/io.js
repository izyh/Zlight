module.exports = {
  'io/connect': (url) => ({
    type: 'io/connect',
    payload: url
  }),
  'io/connect.success': () => ({
    type: 'io/connect.success',
  }),
  'io/connect.failure': (err) => ({
    type: 'io/connect.failure',
    err
  }),
  'io/reconnect.success': () => ({
    type: 'io/reconnect.success'
  }),
  'io/receive.success': (event, data) => ({
    type: 'io/receive.success',
    payload: {event, data}
  })
}
