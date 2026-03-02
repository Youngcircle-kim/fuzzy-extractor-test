const bchAddon = require('./build/Release/bchaddon.node');

const originalHex = 'ffffffffffffffffffffffffffffffff';

const syndrome = bchAddon.generateSyndrome(originalHex);

console.log('syndrome:', syndrome);

const noisyHex = '007fffffffffffffffffffffffffffff';

const recovered = bchAddon.recover(noisyHex, syndrome);

console.log('recovered:', recovered);
