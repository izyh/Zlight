import { handleActions } from 'redux-actions';
import { combineReducer } from 'redux';
import _ from 'lodash';

module.exports = handleActions({

  // fetch
  ['mode/fetch'](state, action) {
    return {...state, loading: true};
  },
  ['mode/fetch.success'](state, action) {
    return {...state, loading: false, mode: action.payload};
  },
  ['mode/fetch.failure'](state, action) {
    return {...state, loading: false, err: action.err};
  },

  // set
  ['mode/set'](state, action) {
    return {...state, loading: true};
  },
  ['mode/set.success'](state, action) {
    return {...state, loading: false, mode: action.payload};
  },
  ['mode/set.failure'](state, action) {
    return {...state, loading: false, err: action.err};
  },

}, {
  mode: '',
  loading: false,
  err: ''
});