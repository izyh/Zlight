import React, { PropTypes } from 'react';
import { Router, Route, IndexRoute, Link, IndexRedirect } from 'react-router';
import Board from '../components/Board';
import NotFound from '../components/NotFound';


const Routes = ({ history }) =>
  <Router history={history}>
    <Route path="/" component={Board} />
  </Router>;

Routes.propTypes = {
  history: PropTypes.any,
};

export default Routes;
