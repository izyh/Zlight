import React, {Component, PropTypes} from 'react';
import { Form, Card, Button, Switch, Input } from 'antd';
import styles from './ListMenu.module.less';

class ListMenu extends Component {
  handleSelect(item, index) {
    this.props.onSelect && this.props.onSelect(item, index);
  }
  render() {
    const { data, loading } = this.props;
    return (
      loading ? <div>载入中...</div> :
      <ul className={styles.list}>
        {
          data.map((item, index) => (
            <li key={item.id} onClick={this.handleSelect.bind(this, item, index)}>
              {item.content}
            </li>
          ))
        }
      </ul>
    )
  }
}
ListMenu.propTypes = {
  data: PropTypes.arrayOf(PropTypes.shape({
    id: PropTypes.any.isRequired,
    content: PropTypes.node.isRequired
  })).isRequired,
  loading: PropTypes.bool.isRequired,
  onSelect: PropTypes.func
}

module.exports = ListMenu;
