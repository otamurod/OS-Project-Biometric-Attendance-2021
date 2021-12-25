const {Pool} = require("pg");
const migrations = require("./index");
const pg = require("pg");
delete pg.native;

const db = new pg.Pool({
	user: "ahmad",
	host: "localhost",
	database: "finger_print",
	password: "root",
	port: 5432,
});

const migrate = async (db) => {
	await db.query(migrations.inspector);
	await db.query(migrations.lecture);
	await db.query(migrations.student);
	await db.query(migrations.attendance);
	process.exit(0);
}

migrate(db);