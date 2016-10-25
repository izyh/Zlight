import xFetch from './xFetch';
import qs from 'querystring';

module.exports = {
  fetchMode () {
    return xFetch('/api/mode');
  },
  setMode (mode) {
    return xFetch('/api/mode', {
      method: 'PUT',
      data: JSON.stringify(mode)
    });
  }
}