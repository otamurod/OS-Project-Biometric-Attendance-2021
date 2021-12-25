const express = require("express");
const router = express.Router();

const admin = require("./admin")
const client = require("./client")

router.use("/admin", admin);
router.use("/client", client);

module.exports = router;