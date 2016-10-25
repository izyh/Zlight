module.exports = {

  // fetch
  'mode/fetch': () => ({
    type: 'mode/fetch',
  }),
  'mode/fetch.success': mode => ({
    type: 'mode/fetch.success',
    payload: mode
  }),
  'mode/fetch.failure': err => ({
    type: 'mode/fetch.failure',
    err
  }),

  // set
  'mode/set': mode => ({
    type: 'mode/set',
    payload: mode
  }),
  'mode/set.success': mode => ({
    type: 'mode/set.success',
    payload: mode
  }),
  'mode/set.failure': err => ({
    type: 'mode/set.failure',
    err
  }),
}