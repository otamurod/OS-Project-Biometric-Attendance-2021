#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <json-c/json.h>

#define PORT 5600
#define SERVER_IP "192.168.43.216"

GtkBuilder *gtkBuilder;

//LOGIN PAGE
GtkWidget *login_page;

GtkWidget *login_button;

GtkWidget *login_username_input;
GtkWidget *login_password_input;


//NEW STUDENT PAGE
GtkWidget *new_student_page;

GtkWidget *submit_new_student_button;
GtkWidget *new_student_page_back_button;

GtkWidget *student_name_input;
GtkWidget *inha_id_input;
GtkWidget *finger_id_input;



//TODO: INSPECTOR PAGE
GtkWidget *inspector_page;

GtkLabel *inspector_name;

GtkWidget *create_new_classsroom_button;
GtkWidget *get_history_button;
GtkWidget *new_student_button;
GtkWidget *inspector_page_back_button;

//TODO: NEW CLASSROOM PAGE
GtkWidget *new_classroom_page;
GtkWidget *new_classroom_page_back_button;

GtkWidget *lecture_name_input;
GtkWidget *room_number_input;
GtkWidget *submit_newclassroom_button;

//TODO:  CLASSROOM PAGE

int classroom_current_page = 1;
int classroom_total_page = 1;

GtkWidget *classroom_page;

GtkWidget *classroom_close_button;
GtkWidget *classroom_update_button;

GtkWidget *classroom_previous_button;
GtkWidget *classroom_next_button;
GtkLabel *classroom_grid_page;

GtkLabel *student_name_grid[10];
GtkLabel *student_id_grid[10];
GtkLabel *student_time_grid[10];


//TODO:  HISTORY PAGE

int history_current_page = 1;
int history_total_page = 1;
int history_lecture_ids[10];

GtkWidget *history_page;

GtkWidget *history_close_button;
GtkWidget *history_update_button;

GtkWidget *history_previous_button;
GtkWidget *history_next_button;
GtkLabel *history_grid_page;

GtkLabel *history_lecture_grid[10];
GtkLabel *history_room_grid[10];
GtkLabel *history_time_grid[10];
GtkWidget *history_lecture_select_button[10];

//<--------------------------------------------------->
//<--------------------------------------------------->


bool is_open_classroom_page_from_hisotry = true;

char error[10] = "error";
char not_found[10] = "not found";


int clientSocket, ret;
struct sockaddr_in serverAddr;
char buffer[1024] = {0};


struct json_object *inspector_id;

int lecture_id_int;

struct json_object *lecture_id;

const struct json_object *empty_token;

void clean_buff(){
	memset(buffer, 0, sizeof(buffer));
}








//TODO: LOGIN ACTIONS

void on_login_button_clicked (){
	char login_url[300] = "/client";

	struct json_object *get_message;

	clean_buff();

	strcat(login_url, "/login?method=post&username=");
	strcat(login_url, gtk_entry_get_text(GTK_ENTRY(login_username_input)));
	strcat(login_url, "&password=");
	strcat(login_url, gtk_entry_get_text(GTK_ENTRY(login_password_input)));

	send(clientSocket, login_url, strlen(login_url), 0);

	if(recv(clientSocket, buffer, 1024, 0) < 0){
			printf("Error in receiving data\n");
	}
	struct json_object *parsed_json;

	parsed_json = json_tokener_parse(buffer);

	json_object_object_get_ex(parsed_json, "message", &get_message);

	int compare_string1;
	int compare_string2;

	compare_string1 = strcmp(error, json_object_get_string(get_message));
	compare_string2 = strcmp(not_found, json_object_get_string(get_message));

	printf("Message: %s \n", json_object_get_string(get_message));

	if(compare_string1 == 0 || compare_string2 == 0) {

		printf("Password or login is incorrect\n");

	} else {
		printf("Logined with status: %s\n", json_object_get_string(get_message));
		json_object_object_get_ex(parsed_json, "id", &inspector_id);
		gtk_label_set_text(inspector_name, json_object_get_string(get_message));
		gtk_widget_hide(login_page);
		gtk_widget_show(inspector_page);
	}
}

void on_inspector_back_button_pressed() {
	gtk_widget_hide(inspector_page);
	gtk_widget_show(login_page);
}

void on_create_new_classroom_button_pressed() {
	gtk_widget_hide(inspector_page);
	gtk_widget_show(new_classroom_page);
}

void on_create_new_classroom_back_button_pressed() {
	gtk_widget_hide(new_classroom_page);
	gtk_widget_show(inspector_page);
}

void on_new_student_button_pressed() {
	gtk_widget_hide(inspector_page);
	gtk_widget_show(new_student_page);

}







//TODO: CLASSROOM ACTIONS

void on_classroom_update_button_pressed() {

	char update_url[400] = "/client";

	struct json_object *get_message;
	struct json_object *get_page;
	struct json_object *get_items;

	clean_buff();

	gtk_widget_set_sensitive(classroom_next_button, false);
	gtk_widget_set_sensitive(classroom_previous_button, false);

	char classroom_current_page_str[100];
	char lecture_id_str[100];

	sprintf(classroom_current_page_str, "%d", classroom_current_page);
	sprintf(lecture_id_str, "%d", lecture_id_int);

	gtk_label_set_text(classroom_grid_page, classroom_current_page_str);

	strcat(update_url, "/attendance?lecture=");
	strcat(update_url, lecture_id_str);
	strcat(update_url, "&page=");
	strcat(update_url, classroom_current_page_str);
	
	send(clientSocket, update_url, strlen(update_url), 0);

	if(recv(clientSocket, buffer, 1024, 0) < 0) {
			printf("Error in receiving data\n");
	}
	struct json_object *parsed_json;

	parsed_json = json_tokener_parse(buffer);

	json_object_object_get_ex(parsed_json, "message", &get_message);

	int compare_string1;
	int compare_string2;

	compare_string1 = strcmp(error, json_object_get_string(get_message));
	compare_string2 = strcmp(not_found, json_object_get_string(get_message));

	printf("Message: %s \n", json_object_get_string(get_message));

	if(compare_string1 == 0 || compare_string2 == 0) {
		printf("Something go wrong!\n");

	} else {
		printf("Classroom updated with status: %s\n", json_object_get_string(get_message));

		json_object_object_get_ex(parsed_json, "page", &get_page);
		classroom_total_page = json_object_get_int(get_page);
		
		json_object_object_get_ex(parsed_json, "items", &get_items);

		int items_length = json_object_array_length(get_items);

		if (classroom_total_page == classroom_current_page || classroom_total_page == 0) {
			gtk_widget_set_sensitive(classroom_next_button, false);
		} else {
			gtk_widget_set_sensitive(classroom_next_button, true);
		}
		
		if (classroom_current_page == 1) {
			gtk_widget_set_sensitive(classroom_previous_button, false);
		} else {
			gtk_widget_set_sensitive(classroom_previous_button, true);
		}

		for(int i = 0; i < items_length; i++) {

			struct json_object *get_items_holder;

			
			struct json_object *get_name;
			struct json_object *get_inha_id;
			struct json_object *get_created_at;

			get_items_holder = json_tokener_parse( json_object_get_string( json_object_array_get_idx(get_items, i)));

			json_object_object_get_ex(get_items_holder, "name", &get_name);
			json_object_object_get_ex(get_items_holder, "inha_id", &get_inha_id);
			json_object_object_get_ex(get_items_holder, "created_at", &get_created_at);

			gtk_label_set_text(student_name_grid[i], json_object_get_string(get_name));
			gtk_label_set_text(student_id_grid[i], json_object_get_string(get_inha_id));
      		gtk_label_set_text(student_time_grid[i], json_object_get_string(get_created_at));
		}

		for(size_t i = items_length; i < 10; i++) {
			gtk_label_set_text(student_name_grid[i],"");
			gtk_label_set_text(student_id_grid[i], "");
      		gtk_label_set_text(student_time_grid[i], "");
		}
	}

}

void on_classroom_close_button_pressed() {

	if (is_open_classroom_page_from_hisotry == true) {
		gtk_widget_hide(classroom_page);
		gtk_widget_show(history_page);
	} else {

		gtk_widget_hide(classroom_page);
		gtk_widget_show(inspector_page);

		char close_url[200] = "/client";

		struct json_object *get_message;

		clean_buff();

		strcat(close_url, "/close_class?method=post&lecture=");
		strcat(close_url, json_object_get_string(lecture_id));

		send(clientSocket, close_url, strlen(close_url), 0);

		if(recv(clientSocket, buffer, 1024, 0) < 0){
				printf("Error in receiving data.\n");
		}
		struct json_object *parsed_json;

		parsed_json = json_tokener_parse(buffer);

		json_object_object_get_ex(parsed_json, "message", &get_message);

		int compare_string1;
		int compare_string2;

		compare_string1 = strcmp(error, json_object_get_string(get_message));
		compare_string2 = strcmp(not_found, json_object_get_string(get_message));

		printf("Message: %s \n", json_object_get_string(get_message));

		if(compare_string1 == 0 || compare_string2 == 0) {

			printf("Something go wrong\n");

		} else {
			printf("Lecture closed with status: %s\n", json_object_get_string(get_message));
		}
	}
}

void on_classroom_previous_button_pressed() {
	classroom_current_page--;
	on_classroom_update_button_pressed();
}

void on_classroom_next_button_pressed() {
	classroom_current_page++;
	on_classroom_update_button_pressed();
}







//TODO: HISTORY ACTIONS

void on_history_update_button_pressed() {

	clean_buff();

	char history_url[400] = "/client";

	struct json_object *get_message;
	struct json_object *get_page;
	struct json_object *get_items;


	gtk_widget_set_sensitive(history_next_button, false);
	gtk_widget_set_sensitive(history_previous_button, false);

	char history_current_page_str[100];

	sprintf(history_current_page_str, "%d", history_current_page);

	gtk_label_set_text(history_grid_page, history_current_page_str);

	strcat(history_url, "/history?inspector=");
	strcat(history_url, json_object_get_string(inspector_id));
	strcat(history_url, "&page=");
	strcat(history_url, history_current_page_str);
	
	send(clientSocket, history_url, strlen(history_url), 0);

	if(recv(clientSocket, buffer, 1024, 0) < 0) {
			printf("Error in receiving data\n");
	}
	struct json_object *parsed_json;

	parsed_json = json_tokener_parse(buffer);

	json_object_object_get_ex(parsed_json, "message", &get_message);

	int compare_string1;
	int compare_string2;

	compare_string1 = strcmp(error, json_object_get_string(get_message));
	compare_string2 = strcmp(not_found, json_object_get_string(get_message));

	printf("Message: %s \n", json_object_get_string(get_message));

	if(compare_string1 == 0 || compare_string2 == 0) {
		printf("Something go wrong!!!\n");

	} else {
		printf("Updated status: %s\n", json_object_get_string(get_message));

		json_object_object_get_ex(parsed_json, "page", &get_page);
		history_total_page = json_object_get_int(get_page);
		json_object_object_get_ex(parsed_json, "items", &get_items);

		int items_length = json_object_array_length(get_items);

		if (history_total_page == history_current_page) {
			gtk_widget_set_sensitive(history_next_button, false);
		} else {
			gtk_widget_set_sensitive(history_next_button, true);
		} 
		
		if (history_current_page == 1) {
			gtk_widget_set_sensitive(history_previous_button, false);
		} else {	
			gtk_widget_set_sensitive(history_previous_button, true);
		}

		for(int i = 0; i < items_length; i++) {

			struct json_object *get_items_holder;

			struct json_object *id;
			struct json_object *get_title;
			struct json_object *get_room;
			struct json_object *get_created_at;

			get_items_holder = json_tokener_parse( json_object_get_string( json_object_array_get_idx(get_items, i)));

			json_object_object_get_ex(get_items_holder, "id", &id);
			json_object_object_get_ex(get_items_holder, "title", &get_title);
			json_object_object_get_ex(get_items_holder, "room", &get_room);
			json_object_object_get_ex(get_items_holder, "created_at", &get_created_at);

			history_lecture_ids[i] = json_object_get_int(id);
			gtk_widget_set_sensitive(history_lecture_select_button[i], true);
			gtk_label_set_text(history_lecture_grid[i], json_object_get_string(get_title));
			gtk_label_set_text(history_room_grid[i], json_object_get_string(get_room));
      		gtk_label_set_text(history_time_grid[i], json_object_get_string(get_created_at));
		}

		for(size_t i = items_length; i < 10; i++) {
			history_lecture_ids[i] = -1;
			gtk_widget_set_sensitive(history_lecture_select_button[i], false);
			gtk_label_set_text(history_lecture_grid[i],"");
			gtk_label_set_text(history_room_grid[i], "");
      		gtk_label_set_text(history_time_grid[i], "");
		}
	}

}

void on_history_lecture_select_button_pressed(GtkWidget *widget, gpointer data) {

	int index = (int) data;

	lecture_id_int = history_lecture_ids[index];

	printf("%d", index);
	gtk_widget_hide(history_page);
	gtk_widget_show(classroom_page);
	on_classroom_update_button_pressed();

}

void on_history_close_button_pressed() {

	gtk_widget_hide(history_page);
	gtk_widget_show(inspector_page);

}

void on_history_previous_button_pressed() {
	history_current_page--;
	on_history_update_button_pressed();
}

void on_history_next_button_pressed() {
	history_current_page++;
	on_history_update_button_pressed();
}









// TODO: NEWCLASSROOM ACTIONS

void on_submit_newclassroom_button_pressed() {
	char create_class_url[300] = "/client";
	clean_buff();

	struct json_object *get_message;

	strcat(create_class_url, "/create_class?method=post&");
	strcat(create_class_url, "&inspector=");
	strcat(create_class_url, json_object_get_string(inspector_id));
	strcat(create_class_url, "&title=");
	strcat(create_class_url, gtk_entry_get_text(GTK_ENTRY(lecture_name_input)));
	strcat(create_class_url, "&room=");
	strcat(create_class_url, gtk_entry_get_text(GTK_ENTRY(room_number_input)));

	send(clientSocket, create_class_url, strlen(create_class_url), 0);

	if(recv(clientSocket, buffer, 1024, 0) < 0){
		printf("Error in receiving data.\n");
	}
	struct json_object *parsed_json;

	parsed_json = json_tokener_parse(buffer);

	json_object_object_get_ex(parsed_json, "message", &get_message);

	int compare_string1;
	int compare_string2;

	compare_string1 = strcmp(error, json_object_get_string(get_message));
	compare_string2 = strcmp(not_found, json_object_get_string(get_message));

	printf("Message: %s\n", json_object_get_string(get_message));

	if(compare_string1 == 0 || compare_string2 == 0) {

		printf("Credeintials are incorrect\n");

	} else {

		json_object_object_get_ex(parsed_json, "id", &lecture_id);
		lecture_id_int = json_object_get_int(lecture_id);
		printf("Class Created ID: %s\n", json_object_get_string(lecture_id));
		gtk_widget_hide(new_classroom_page);
		gtk_widget_show(classroom_page);
		is_open_classroom_page_from_hisotry = false;
		on_classroom_update_button_pressed();

	}
}

void on_get_history_button_pressed() {
	gtk_widget_hide(inspector_page);
	gtk_widget_show(history_page);
	is_open_classroom_page_from_hisotry = true;
	on_history_update_button_pressed();
}






//TODO: NEW STUDENT ACTIONS

void on_submit_new_student_button_pressed() {
	char create_new_student_url[300] = "/client";
	clean_buff();

	struct json_object *get_message;

	strcat(create_new_student_url, "/create_student?method=post&inha_id=");
	strcat(create_new_student_url, gtk_entry_get_text(GTK_ENTRY(inha_id_input)));
	strcat(create_new_student_url, "&finger_id=");
	strcat(create_new_student_url, gtk_entry_get_text(GTK_ENTRY(finger_id_input)));
	strcat(create_new_student_url, "&name=");
	strcat(create_new_student_url, gtk_entry_get_text(GTK_ENTRY(student_name_input)));

	send(clientSocket, create_new_student_url, strlen(create_new_student_url), 0);

	if(recv(clientSocket, buffer, 1024, 0) < 0){
		printf("Error in receiving data.\n");
	}
	struct json_object *parsed_json;

	parsed_json = json_tokener_parse(buffer);

	json_object_object_get_ex(parsed_json, "message", &get_message);

	int compare_string1;
	int compare_string2;

	compare_string1 = strcmp(error, json_object_get_string(get_message));
	compare_string2 = strcmp(not_found, json_object_get_string(get_message));

	if(compare_string1 == 0 || compare_string2 == 0) {

		printf("Such student has already exist\n");

	} else {

		struct json_object *student_id;
		json_object_object_get_ex(parsed_json, "id", &student_id);
		lecture_id_int = json_object_get_int(lecture_id);
		printf("Student created id: %s\n", json_object_get_string(student_id));
		gtk_widget_hide(new_student_page);
		gtk_widget_show(inspector_page);

	}
}

void on_new_student_page_back_button_pressed() {
	gtk_widget_hide(new_student_page);
	gtk_widget_show(inspector_page);
}



int main(int argc, char *argv[])
{
  gtk_init(&argc, &argv);

	// new code
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("<< + >> SOCKET CONNECTED\n");
		exit(1);
	}
	printf("<< + >> SOCKET CREATED.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("<< - >> SOCKET HAS NOT CREATED\n");
		exit(1);
	}
	printf("<< + >> CONNECTED TO THE SERVER\n");

	gtkBuilder = gtk_builder_new();
	gtk_builder_add_from_file(gtkBuilder, "client.glade", NULL);

	//TODO:  SET LOGIN PAGE
	login_page = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "login_page"));

  	login_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "login_button"));
	g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), NULL);

	login_username_input = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "login_username_input"));
	login_password_input = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "login_password_input"));

	//TODO:  SET INSPECTOR PAGES
	inspector_page = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "inspector_page"));

	inspector_name = GTK_LABEL(gtk_builder_get_object(gtkBuilder, "inspector_name"));

	create_new_classsroom_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "create_new_classsroom_button"));
	g_signal_connect(create_new_classsroom_button, "clicked", G_CALLBACK(on_create_new_classroom_button_pressed), NULL);

	get_history_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "get_history_button"));
	g_signal_connect(get_history_button, "clicked", G_CALLBACK(on_get_history_button_pressed), NULL);

	new_student_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "new_student_button"));
	g_signal_connect(new_student_button, "clicked", G_CALLBACK(on_new_student_button_pressed), NULL);

	inspector_page_back_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "inspector_page_back_button"));
	g_signal_connect(inspector_page_back_button, "clicked", G_CALLBACK(on_inspector_back_button_pressed), NULL);


	//TODO:  SET NEW CLASSROOM PAGE
	new_classroom_page = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "new_classroom_page"));

	new_classroom_page_back_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "new_classroom_page_back_button"));
	g_signal_connect(new_classroom_page_back_button, "clicked", G_CALLBACK(on_create_new_classroom_back_button_pressed), NULL);

	submit_newclassroom_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "submit_newclassroom_button"));
	g_signal_connect(submit_newclassroom_button, "clicked", G_CALLBACK(on_submit_newclassroom_button_pressed), NULL);

	lecture_name_input = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "lecture_name_input"));
	room_number_input = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "room_number_input"));



	//TODO:  SET NEW STUDENT PAGE
	new_student_page = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "new_student_page"));

	new_student_page_back_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "new_student_page_back_button"));
	g_signal_connect(new_student_page_back_button, "clicked", G_CALLBACK(on_new_student_page_back_button_pressed), NULL);

	submit_new_student_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "submit_new_student_button"));
	g_signal_connect(submit_new_student_button, "clicked", G_CALLBACK(on_submit_new_student_button_pressed), NULL);

	student_name_input = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "student_name_input"));
	inha_id_input = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "inha_id_input"));
	finger_id_input = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "finger_id_input"));


	//TODO: -  SET CLASSROOM PAGE 

	classroom_page = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "classroom_page"));

	classroom_grid_page = GTK_LABEL(gtk_builder_get_object(gtkBuilder, "classroom_grid_page"));

	classroom_close_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "classroom_close_button"));
	g_signal_connect(classroom_close_button, "clicked", G_CALLBACK(on_classroom_close_button_pressed), NULL);

	classroom_update_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "classroom_update_button"));
	g_signal_connect(classroom_update_button, "clicked", G_CALLBACK(on_classroom_update_button_pressed), NULL);

	classroom_previous_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "classroom_previous_button"));
	g_signal_connect(classroom_previous_button, "clicked", G_CALLBACK(on_classroom_previous_button_pressed), NULL);

	classroom_next_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "classroom_next_button"));
	g_signal_connect(classroom_next_button, "clicked", G_CALLBACK(on_classroom_next_button_pressed), NULL);


	for (int i = 0; i < 10; i++) {
		char student_name_grid_str[22] = "student_name_grid_";
		char student_id_grid_str[22] = "student_id_grid_";
		char student_time_grid_str[22] = "student_time_grid_";

		char item_index_holder[2];
		sprintf(item_index_holder, "%d", i);

		strcat(student_name_grid_str, item_index_holder);
		strcat(student_id_grid_str, item_index_holder);
		strcat(student_time_grid_str, item_index_holder);

		student_name_grid[i] = GTK_LABEL(gtk_builder_get_object(gtkBuilder, student_name_grid_str));
		student_id_grid[i] = GTK_LABEL(gtk_builder_get_object(gtkBuilder, student_id_grid_str));
		student_time_grid[i] = GTK_LABEL(gtk_builder_get_object(gtkBuilder, student_time_grid_str));
	}


	//TODO: -  SET HISTORY PAGE 

	history_page = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "history_page"));

	history_grid_page = GTK_LABEL(gtk_builder_get_object(gtkBuilder, "history_grid_page"));

	history_close_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "history_close_button"));
	g_signal_connect(history_close_button, "clicked", G_CALLBACK(on_history_close_button_pressed), NULL);

	history_update_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "history_update_button"));
	g_signal_connect(history_update_button, "clicked", G_CALLBACK(on_history_update_button_pressed), NULL);

	history_previous_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "history_previous_button"));
	g_signal_connect(history_previous_button, "clicked", G_CALLBACK(on_history_previous_button_pressed), NULL);

	history_next_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "history_next_button"));
	g_signal_connect(history_next_button, "clicked", G_CALLBACK(on_history_next_button_pressed), NULL);


	for (int i = 0; i < 10; i++) {
		char history_lecture_grid_str[22] = "history_lecture_grid_";
		char history_room_grid_str[22] = "history_room_grid_";
		char history_time_grid_str[22] = "history_time_grid_";
		char history_lecture_select_button_str[34] = "history_lecture_select_button_";

		char item_index_holder[2];
		sprintf(item_index_holder, "%d", i);

		strcat(history_lecture_select_button_str, item_index_holder);
		strcat(history_lecture_grid_str, item_index_holder);
		strcat(history_room_grid_str, item_index_holder);
		strcat(history_time_grid_str, item_index_holder);

		history_lecture_select_button[i] = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, history_lecture_select_button_str));	
		g_signal_connect(history_lecture_select_button[i], "clicked", G_CALLBACK(on_history_lecture_select_button_pressed), i);
		history_lecture_grid[i] = GTK_LABEL(gtk_builder_get_object(gtkBuilder, history_lecture_grid_str));
		history_room_grid[i] = GTK_LABEL(gtk_builder_get_object(gtkBuilder, history_room_grid_str));
		history_time_grid[i] = GTK_LABEL(gtk_builder_get_object(gtkBuilder, history_time_grid_str));
	}


	g_object_unref(G_OBJECT(gtkBuilder));
	gtk_widget_show(login_page);
	gtk_main();

	return 0;
}
