module.exports = {
	"inspector": `
		CREATE TABLE IF NOT EXISTS inspector(
			id SERIAL PRIMARY KEY,
			name VARCHAR(255) UNIQUE NOT NULL,
			password VARCHAR(255) NOT NULL,
			username VARCHAR(255) UNIQUE NOT NULL,
			card_id VARCHAR(255) UNIQUE NOT NULL
		);
	`,
	"lecture": `
		CREATE TABLE IF NOT EXISTS lecture(
			id SERIAL PRIMARY KEY,
			inspector INTEGER NOT NULL,
			created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
			title VARCHAR(255) NOT NULL,
			room VARCHAR(255) NOT NULL,
			is_active BOOL NOT NULL,
			FOREIGN KEY(inspector) 
				REFERENCES inspector(id)
		);
	`,
	"student": `
		CREATE TABLE IF NOT EXISTS student(
			id SERIAL PRIMARY KEY,
			inha_id VARCHAR(255) UNIQUE NOT NULL,
			finger_id integer UNIQUE NOT NULL,
			name VARCHAR(255) NOT NULL
		);
	`,
	"attendance": `
		CREATE TABLE IF NOT EXISTS attendance(
			id SERIAL PRIMARY KEY,
			created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
			student INTEGER NOT NULL,
			lecture INTEGER NOT NULL,
			FOREIGN KEY(student)
				REFERENCES student(id),
			FOREIGN KEY(lecture)
				REFERENCES lecture(id),
			UNIQUE(student, lecture)
		);
	`
}