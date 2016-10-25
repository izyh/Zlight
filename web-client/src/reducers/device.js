import { handleActions } from 'redux-actions';
import { combineReducer } from 'redux';
import _ from 'lodash';

module.exports = handleActions({

  ['device/fetch/all'](state, action) {
    return {...state, loading: true}
  },
  ['device/fetch/all.success'](state, action) {
    return {...state, loading: false, devices: action.payload}
  },
  ['device/fetch/all.failure'](state, action) {
    return {...state, loading: false, err: action.err}
  },

  // fetch one
  ['device/fetch/one'](state, action) {
    return {...state, fetchOneLoading: true}
  },
  ['device/fetch/one.success'](state, action) {
    const { nwk } = action.payload;
    const devices = _.chain(state.devices)
      .map(dev => {
        if (dev.nwk == nwk) { return action.payload }
        else { return dev }
      })
      .value();
    return {...state, fetchOneLoading: false, devices}
  },
  ['device/fetch/one.failure'](state, action) {
    return {...state, fetchOneLoading: false, fetchOneErr: action.err}
  },

  // prop set
  ['device/app/prop/set'](state, action) {
    return {...state, appSetLoading: true}
  },
  ['device/app/prop/set.success'](state, action) {
    const rApp = action.payload;
    const devices =  _.chain(state.devices)
      .map(dev => {
        const newApps = _.map(dev.apps, app => {
          if ((rApp.device == app.device) && (rApp.endPoint == app.endPoint)) {
            return rApp;
          } else { return app }
        });
        return {...dev, apps: newApps }
      })
      .value();
    return {...state, devices, appSetLoading: false}
  },
  ['device/app/prop/set.failure'](state, action) {
    return {...state, appSetErr: action.err, appSetLoading: false}
  }
}, {
  devices: [],
  loading: false,
  err: null,
  // fetch one
  fetchOneLoading: false,
  fetchOneErr: null,
  // app set
  appSetLoading: false,
  appSetErr: null,
});
