import { handleActions } from 'redux-actions';
import { combineReducer } from 'redux';
import _ from 'lodash';

module.exports = handleActions({
  ['io/connect'](state, action) {
    return {...state, url: action.payload};
  },
  ['io/connect.success'](state, action) {
    return {...state };
  },
  ['io/connect.failure'](state, action) {
    return {...state, err: action.err}
  }
}, {
  url: '',
  err: null
});
