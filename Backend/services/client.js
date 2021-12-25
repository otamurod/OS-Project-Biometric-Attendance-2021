const express = require("express");

const client = express.Router();

const db = require("../database")


client.post("/login", async (req, res) => {
	let {username, password} = req.query;

	try {
		let inspector = (await db.query(`
			SELECT name, id
			FROM inspector
			WHERE username = $1 AND password = $2
		`, [username, password])).rows[0];

		if(!inspector) throw new Error("error");

		res.json({message: inspector.name, id: inspector.id});

	} catch (e) {
		res.status(406).json({message: "error"})
	}
})


client.post("/close_class", async (req, res) => {
	let {lecture} = req.query;

	try {
		let id = (await db.query(`
			UPDATE lecture
			SET is_active = false
			WHERE id = $1
			RETURNING id
		`, [lecture]))
		
		if(!id) throw new Error("error");

		res.json({message: "success"});

	} catch (e) {
		console.log(e.message)
		res.status(406).json({message: "error"})
	}
})

client.get("/attendance", async (req, res) => {

	let {lecture, page} = req.query;
	let offset = (page - 1) * 10;

	try {
		let page = (await db.query(`
			SELECT ceil(count(id)::decimal / 10) as page
			FROM attendance
			WHERE lecture = $1
		`, [lecture])).rows[0].page

		let items = (await db.query(`
			SELECT s.name, s.inha_id, a.created_at
			FROM attendance a
			LEFT JOIN student s ON s.id = a.student
			WHERE lecture = $1
			limit 10 offset $2
		`, [lecture, offset])).rows

		items.forEach(item => {
			item.created_at = new Date(item.created_at).toLocaleString()
		})

		if(!page) throw new Error("error");

		res.json({
			message: "success",
			page,
			items
		});

	} catch (e) {
		console.log(e.message);
		res.status(406).json({message: "error"})
	}
})

client.get("/history", async (req, res) => {

	let {inspector, page} = req.query;
	let offset = (page - 1) * 10;

	try {
		let page = (await db.query(`
			SELECT ceil(count(id)::decimal / 10) as page
			FROM lecture
			WHERE inspector = $1
		`, [inspector])).rows[0].page

		let items = (await db.query(`
			SELECT title, created_at, room, id
			FROM lecture
			WHERE inspector = $1
			ORDER BY created_at desc
			limit 10 offset $2
		`, [inspector, offset])).rows

		items.forEach(item => {
			item.created_at = new Date(item.created_at).toLocaleString()
		})

		if(!page) throw new Error("error");

		res.json({
			message: "success",
			page,
			items
		});

	} catch (e) {
		console.log(e.message);
		res.status(406).json({message: "error"})
	}
})

client.post("/create_class", async (req, res) => {
	let {inspector, title, room} = req.query;

	try {
		await db.query(`
			UPDATE lecture
			SET is_active = false
			WHERE id IN (
				SELECT id 
				FROM lecture
				WHERE is_active = true and inspector = $1
			)
		`, [inspector])

		let lecture_id = (await db.query(`
			INSERT INTO lecture(inspector, title, room, is_active)
			VALUES ($1, $2, $3, true)
			RETURNING id
		`, [inspector, title, room])).rows[0].id;

		if(!lecture_id) throw new Error("error");

		res.json({message: "success", id: lecture_id});

	} catch (e) {
		res.status(406).json({message: "error"})
	}
})


client.post("/create_student", async (req, res) => {
	let {inha_id, finger_id, name} = req.query;

	try {
		let student_id = (await db.query(`
			INSERT INTO student(inha_id, finger_id, name)
			VALUES ($1, $2, $3)
			RETURNING id
		`, [inha_id, finger_id, name])).rows[0].id;

		if(!student_id) throw new Error("error");

		res.json({message: "success"});

	} catch (e) {
		res.status(406).json({message: "error"})
	}
})

client.get("/check_finger", async (req, res) => {

	let {card_id, finger_id} = req.query;
	
	try {
		let student = (await db.query(`
			INSERT INTO attendance(student, lecture)
			SELECT s.id as student, l.id as lecture
			FROM lecture l
					LEFT JOIN inspector i on i.id = l.inspector
					CROSS JOIN student s
			WHERE s.finger_id = $2
			AND l.is_active = true
			AND i.card_id = $1
			RETURNING (
				SELECT inha_id
				FROM student
				WHERE finger_id = $2
			)
			`, [card_id, finger_id])).rows[0];

        	if(!student) throw new Error("error");

        	res.json({message: student.inha_id})

	} catch (e) {
		if (e.code == 23505) {
			res.status(200).json({message: "You are in a class"})
		} else {
			res.status(406).json({message: "error"})
		}
	}
})

client.get("/check_inspector", async (req, res) => {

	let {card_id} = req.query;
	
	try {
		let inspector = (await db.query(`
			SELECT name
			FROM inspector
			WHERE card_id = $1
		`, [card_id])).rows[0];

		if(!inspector) throw new Error("error");

        res.json({inspector_name: inspector.name})

	} catch (e) {
		res.status(406).json({message: "error"})
	}
})

client.get("/get_id", async (req, res) => {
	
	try {
		let inspector = (await db.query(`
			SELECT max(finger_id)
			FROM student
		`)).rows[0];

		if(!inspector) throw new Error("error");

        res.json({finger_id: inspector.max + 1})

	} catch (e) {
		res.status(406).json({message: "error"})
	}
})


module.exports = client;