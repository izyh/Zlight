import React, { PropTypes } from 'react';
import { Router, Route, IndexRoute, Link, IndexRedirect, Redirect } from 'react-router';
import Container from '../components/Scene/Container';
import StaticScene from '../components/Scene/StaticScene';
import NotFound from '../components/NotFound';

const Routes = ({ history }) =>
  <Router history={history}>
    <Route path="/" component={Container}>
      <IndexRedirect to="/static" />
      <Route path="/static" component={StaticScene} />
    </Route>
    <Route path="*" component={NotFound} />
  </Router>;

Routes.propTypes = {
  history: PropTypes.any,
};

export default Routes;
