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
#define SERVER_IP "192.168.31.194"

GtkBuilder *gtkBuilder;

//LOGIN PAGE
GtkWidget *login_page;

GtkWidget *login_button;

GtkWidget *login_username_input;
GtkWidget *login_password_input;


//NEW INSPECTOR PAGE
GtkWidget *new_inspector_page;

GtkWidget *submit_new_inspector_button;
GtkWidget *new_inspector_page_back_button;

GtkWidget *inspector_name_input;
GtkWidget *inspector_username_input;
GtkWidget *inspector_password_input;
GtkWidget *inspector_confirm_password_input;
GtkWidget *inspector_id_card_input;



//TODO: ADMIN PAGE
GtkWidget *admin_page;

GtkWidget *create_new_inspector_button;
GtkWidget *admin_page_back_button;


//<--------------------------------------------------->
//<--------------------------------------------------->

char error[10] = "error";
char not_found[10] = "not found";


int clientSocket, ret;
struct sockaddr_in serverAddr;
char buffer[1024] = {0};


struct json_object *token;

void clean_buff(){
	memset(buffer, 0, sizeof(buffer));
}








//TODO: LOGIN ACTIONS

void on_login_button_clicked (){
	char login_url[300] = "/admin";

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
		json_object_object_get_ex(parsed_json, "token", &token);
		gtk_widget_hide(login_page);
		gtk_widget_show(admin_page);
	}
}

void on_amdin_back_button_pressed() {
	gtk_widget_hide(admin_page);
	gtk_widget_show(login_page);
}

void on_create_new_inspector_button_pressed() {
	gtk_widget_hide(admin_page);
	gtk_widget_show(new_inspector_page);
}

void on_create_new_inspector_back_button_pressed() {
	gtk_widget_hide(new_inspector_page);
	gtk_widget_show(admin_page);
}


// TODO: NEWCLASSROOM ACTIONS

void on_submit_new_inspector_button_pressed() 
{
	int compare;
	compare = strcmp(gtk_entry_get_text(GTK_ENTRY(inspector_password_input)), gtk_entry_get_text(GTK_ENTRY(inspector_confirm_password_input)));
	if (compare == 0)
	{
		char create_class_url[300] = "/admin";
		clean_buff();

		struct json_object *get_message;

		strcat(create_class_url, "/create_inspector?method=post&");
		strcat(create_class_url, "&token=");
		strcat(create_class_url, json_object_get_string(token));
		strcat(create_class_url, "&name=");
		strcat(create_class_url, gtk_entry_get_text(GTK_ENTRY(inspector_name_input)));
		strcat(create_class_url, "&username=");
		strcat(create_class_url, gtk_entry_get_text(GTK_ENTRY(inspector_username_input)));
		strcat(create_class_url, "&password=");
		strcat(create_class_url, gtk_entry_get_text(GTK_ENTRY(inspector_password_input)));
		strcat(create_class_url, "&id_card=");
		strcat(create_class_url, gtk_entry_get_text(GTK_ENTRY(inspector_id_card_input)));

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

			gtk_widget_hide(new_inspector_page);
			gtk_widget_show(admin_page);

		}
	} else {
		printf("Passwords are not the same\n");
	}
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
	gtk_builder_add_from_file(gtkBuilder, "admin.glade", NULL);

	//TODO:  SET LOGIN PAGE
	login_page = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "login_page"));

  	login_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "login_button"));
	g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), NULL);

	login_username_input = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "login_username_input"));
	login_password_input = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "login_password_input"));

	//TODO:  SET ADMIN PAGES
	admin_page = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "admin_page"));


	create_new_inspector_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "create_new_inspector_button"));
	g_signal_connect(create_new_inspector_button, "clicked", G_CALLBACK(on_create_new_inspector_button_pressed), NULL);


	admin_page_back_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "admin_page_back_button"));
	g_signal_connect(admin_page_back_button, "clicked", G_CALLBACK(on_amdin_back_button_pressed), NULL);


	//TODO:  SET NEW INSPECTOR PAGE
	new_inspector_page = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "new_inspector_page"));

	new_inspector_page_back_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "new_inspector_page_back_button"));
	g_signal_connect(new_inspector_page_back_button, "clicked", G_CALLBACK(on_create_new_inspector_back_button_pressed), NULL);

	submit_new_inspector_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "submit_new_inspector_button"));
	g_signal_connect(submit_new_inspector_button, "clicked", G_CALLBACK(on_submit_new_inspector_button_pressed), NULL);

	inspector_name_input = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "inspector_name_input"));
	inspector_username_input = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "inspector_username_input"));
	inspector_password_input = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "inspector_password_input"));
	inspector_confirm_password_input = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "inspector_confirm_password_input"));
	inspector_id_card_input = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "inspector_id_card_input"));

	g_object_unref(G_OBJECT(gtkBuilder));
	gtk_widget_show(login_page);
	gtk_main();

	return 0;
}
