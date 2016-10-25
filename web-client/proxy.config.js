// Learn more on how to config.
// - https://github.com/dora-js/dora-plugin-proxy#规则定义

let store = {
  lamps: {
    1: {id: 1, name: "lamp1", level: 1},
    2: {id: 2, name: "lamp2", level: 2},
  }
};

let count = 1;
for(count=1; count<50; count++) {
  store.lamps[count] = {
    id: count,
    name: "lamp-"+count,
    level: Math.random()*100
  }
}

const handleLamp = function(req, res) {
  const method = req.method.toLowerCase();
  if (method=="get") {
    res.json(store.lamps);
  } else {
    res.status(401).end();
  }
};

const handleLampOne = function (req, res) {
  const method = req.method.toLowerCase();
  const lid = req.params.id;
  let lampOne = store.lamps[lid];
  if (lampOne) {
    if (method=="get") {
      res.json(lampOne);
    } else if (method=="post") {
      const data = JSON.parse(req.body);
      Object.assign(lampOne, data);
      res.json(lampOne);
    } else if (method=="delete") {
      delete store.lamps[lid];
      res.status(200).end();
    }
  } else {
    res.status(404).end();
  }
}

const handleSse = function (req, res) {
  console.dir(res);
  setTimeout(() => {
    res.send('event:ping\n\n');
  }, 2000);
}

// export
// -------------------------------
// module.exports = {
//   '/api/lamps': handleLamp,
//   '/api/lamps/:id': handleLampOne,
// };

module.exports = {
  '/api/*': 'http://localhost:3000/',
  '/socket.io/*': 'http://localhost:3000/',
};
