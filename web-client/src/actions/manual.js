module.exports = {
  'manual/remote/set': (nwk, ep, payload) => ({
    type: 'manual/remote/set',
    payload: {nwk, ep, payload}
  }),
  'manual/remote/set.success': (nwk, ep, app) => ({
    type: 'manual/remote/set.success',
    payload: {nwk, ep, app}
  }),
  'manual/remote/set.failure': err => ({
    type: 'manual/remote/set.failure',
    err,
  })
}