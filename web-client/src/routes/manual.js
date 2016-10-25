import React, { PropTypes } from 'react';
import { Router, Route, IndexRoute, Link, IndexRedirect } from 'react-router';
import CtrlPanel from '../components/CtrlPanel/Panel';
import NotFound from '../components/NotFound';


const Routes = ({ history }) =>
  <Router history={history}>
    <Route path="/ctrl/:type" component={CtrlPanel} />
  </Router>;

Routes.propTypes = {
  history: PropTypes.any,
};

export default Routes;
