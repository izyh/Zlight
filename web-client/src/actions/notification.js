module.exports = {
  'notification/success': (m, d) => ({
    type: 'notification/success',
    payload: {
      message: m,
      description: d
    }
  }),
  'notification/error': (m, d) => ({
    type: 'notification/error',
    payload: {
      message: m,
      description: d
    }
  }),
}