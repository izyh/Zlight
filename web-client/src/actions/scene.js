
module.exports = {
  // fetch static
  'scene/fetch/static': () => ({
    type: 'scene/fetch/static'
  }),
  'scene/fetch/static.success': list => ({
    type: 'scene/fetch/static.success',
    payload: list
  }),
  'scene/fetch/static.failure': err => ({
    type: 'scene/fetch/static.failure',
    err
  }),

  // change scene
  'scene/modify/static': scene => ({
    type: 'scene/modify/static',
    payload: scene
  }),
  'scene/modify/static.success': (scene) => ({
    type: 'scene/modify/static.success',
    payload: scene
  }),
  'scene/modify/static.failure': (err) => ({
    type: 'scene/modify/static.failure',
    err
  }),
};