const express = require("express");
const jwt = require('jsonwebtoken');
const jwt_decode = require('jwt-decode');

const admin = express.Router();

const db = require("../database")

admin.post("/login", async (req, res) => {
	let {username, password} = req.query;

	try {
		
		if(username == "admin" && password == "adminadmin") {
            res.json({message: "success", token: "aht32t2gserry438cm"});
        } else {
            throw new Error("error");  
        }


	} catch (e) {
		res.status(406).json({message: "error"})
	}
})

admin.post("/create_inspector", async (req, res) => {
	let {token, name, username, password, id_card} = req.query;

	try {
        if (token == "aht32t2gserry438cm") {
		let inspector_id = (await db.query(`
			INSERT INTO inspector(name, username, password, card_id)
			VALUES ($1, $2, $3, $4)
			RETURNING id
		`, [name, username, password, id_card])).rows[0].id;

		if(!inspector_id) throw new Error("error");

		res.json({message: "success"});
        } else {
            throw new Error("error");
        }

	} catch (e) {
		res.status(406).json({message: "error"})
	}
})

module.exports = admin
