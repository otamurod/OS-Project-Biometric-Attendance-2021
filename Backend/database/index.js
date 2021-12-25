const pg = require("pg");
delete pg.native;

const db = new pg.Pool({
	user: "ahmad",
	host: "localhost",
	database: "finger_print",
	password: "root",
	port: 5432,
});

db.query(`
  SET TIMEZONE='Asia/Tashkent'
`);

module.exports = db;