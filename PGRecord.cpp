

#include "PGRecord.h"

///Constructores

PGRecord::PGRecord(){
    this->mostrar_log = false;
    this->existe_con = false;
    this->conexion = NULL;
    this->resultado = NULL;
    this->tabla_en_uso = "";
    this->host = "localhost";
    this->db = "postgres";
    this->usuario = "postgres";
    this->puerto = "5432";
    this->password = "";
    this->opciones = "";
    this->tty = "";
}

PGRecord::PGRecord(string host, string puerto, string db, string usuario, string password){
    this->mostrar_log = false;
    this->existe_con = false;
    this->conexion = NULL;
    this->resultado = NULL;
    this->tabla_en_uso = "";
    this->host = host;
    this->db = db;
    this->usuario = usuario;
    this->puerto = puerto;
    this->password = password;
    this->opciones = "";
    this->tty = "";
}

PGRecord::PGRecord(string host, string puerto, string opciones, string tty,string db, string usuario, string password){
    this->mostrar_log = false;
    this->existe_con = false;
    this->conexion = NULL;
    this->resultado = NULL;
    this->tabla_en_uso = "";
    this->host = host;
    this->db = db;
    this->usuario = usuario;
    this->puerto = puerto;
    this->password = password;
    this->opciones = opciones;
    this->tty = tty;
}

PGRecord::~PGRecord(){
    this->cerrar_conexion();
    //this->liberar_resultado();
}


///Metodos set >parametros de conexion<
void PGRecord::set_host(string _host){
    this->host = _host;
}
void PGRecord::set_puerto(string _puerto){
    this->puerto = _puerto;
}
void PGRecord::set_opciones(string _opciones){
    this->opciones = _opciones;
}
void PGRecord::set_tty(string _tty){
    this->tty = _tty;
}
void PGRecord::set_db(string _db){
    this->db = _db;
}
void PGRecord::set_usuario(string _usuario){
    this->usuario = _usuario;
}
void PGRecord::set_password(string _password){
    this->password = _password;
}
void PGRecord::set_datos_conexion(string _host, string _puerto, string _db, string _usuario, string _password){
    this->host = _host;
    this->puerto = _puerto;
    this->db = _db;
    this->usuario = _usuario;
    this->password = _password;
}


///Metodos get >parametros de conexion<

char *PGRecord::get_host(){
    return (this->existe_con)?(PQhost(this->conexion)):((char*)"NULL");
}
char *PGRecord::get_puerto(){
    return (this->existe_con)?(PQport(this->conexion)):((char*)"");
}
char *PGRecord::get_opciones(){
    return (this->existe_con)?(PQoptions(this->conexion)):((char*)"");
}
char *PGRecord::get_tty(){
    return (this->existe_con)?(PQtty(this->conexion)):((char*)"");
}
char *PGRecord::get_db(){
    return (this->existe_con)?(PQdb(this->conexion)):((char*)"");
}
char *PGRecord::get_usuario(){
    return (this->existe_con)?(PQuser(this->conexion)):((char*)"");
}
char *PGRecord::get_password(){
    return (this->existe_con)?(PQpass(this->conexion)):((char*)"");
}
/*
char *PGRecord::get_datos_conexion(){
    int tamano = (sizeof(this->get_host()))+(sizeof(this->get_puerto()))+(sizeof(this->get_opciones()))+(sizeof(this->get_tty()))+(sizeof(this->get_db()))+(sizeof(this->get_usuario()));

    return crear_cadena(\
        "Host: %s \n Puerto: %s \n Opciones: %s\n tty: %s \n DB: %s \n Usuario: %s\n"\
            ,tamano,this->get_host(),this->get_puerto(), this->get_opciones(), this->get_tty(),this->get_db(),this->get_usuario());

}
*/
///Metodo para mostrar mensajes de los arrojados por los metodos
void PGRecord::log(bool estado){
    this->mostrar_log = estado;
}
void PGRecord::imprimir_log(string mensaje){
    if(this->mostrar_log){
        printf("%s \n", mensaje.c_str());
    }
}


///Metodo para conectar con la base de datos
bool PGRecord::conectar(){
    this->conexion = PQsetdbLogin(this->host.c_str(),this->puerto.c_str(),NULL,NULL,this->db.c_str(),this->usuario.c_str(),this->password.c_str());
    if(this->estado() == CONEXION_FALLIDA){
        this->cerrar_conexion();
        this->imprimir_log("Error, no se pudo conectar con la base de datos");
        exit(EXIT_SUCCESS);
        return false;
    }
    else{
        this->existe_con = true;
        this->imprimir_log("Exito, Conexion realizada con exito");
        return true;
    }
}

void PGRecord::cerrar_conexion(){
    this->existe_con = false;
    PQfinish(this->conexion);
}

void PGRecord::reiniciar_conexion(){
    PQreset(this->conexion);
}

void PGRecord::liberar_resultado(){
    PQclear(this->resultado);
    this->imprimir_log("Resultado borrado con exito..");
}

int PGRecord::get_id_proceso(){
    return PQbackendPID(this->conexion);
}

///Metodo para saber el estado de la conexion
int PGRecord::estado(){
    return PQstatus(this->conexion);
}

///Metodo para consultar a la base de datos
char *PGRecord::consulta(string  query__){

    if(this->existe_con){
        this->resultado = PQexec(this->conexion, query__.c_str());

        return this->comprueba_estado_consulta(PQresultStatus(this->resultado));
    }

    return NULL;
}

///Metodos para la interaccion directa con una sola tabla
void PGRecord::usar_tabla(string nombre_tabla__){
    if(nombre_tabla__ != ""){
        this->tabla_en_uso = nombre_tabla__;
        return;
    }
    this->imprimir_log("Error, No se ha proporcionado el nombre de la tabla");
    exit(EXIT_SUCCESS);
}

///Seleccionar todos los camposs
char *PGRecord::seleccionar(string opcion__){

    if(this->tabla_en_uso != ""){

        string *consulta_tmp = new string;

        if(opcion__ == "*"){
            consulta_tmp->append(crear_cadena("SELECT  *  from   %s  ;",this->tabla_en_uso.c_str()));
        }
        else if((atoi(opcion__.c_str()) != 0) && opcion__ != "*"){
            consulta_tmp->append(crear_cadena("SELECT * FROM %s LIMIT %s;",this->tabla_en_uso.c_str(),opcion__.c_str()));
        }
        else{
            this->imprimir_log(crear_cadena("Error, Parametro invalido no se reconoce > '%s'",opcion__.c_str()));
            delete consulta_tmp;
            return NULL;
        }

        this->resultado = PQexec(this->conexion, consulta_tmp->c_str());
        delete consulta_tmp;
        return this->comprueba_estado_consulta(PQresultStatus(this->resultado));
    }
    else{
        this->imprimir_log("Error, No se ha seleccionado una tabla para ejecutar esta funcion");
        return NULL;
    }
    return NULL;
}

///Seleccionar los campos especificados
char *PGRecord::seleccionar(string campos__, string opcion__){

    if(this->tabla_en_uso != ""){

        string consulta_tmp;

        if((opcion__ == "*") && (campos__ != "")){

            consulta_tmp = crear_cadena("SELECT  %s  from   %s  ;",campos__.c_str(),this->tabla_en_uso.c_str());
        }
        else if((atoi(opcion__.c_str()) != 0) && (opcion__ != "*") && campos__ != ""){
            consulta_tmp = crear_cadena("SELECT %s FROM %s LIMIT %s;",campos__.c_str(),this->tabla_en_uso.c_str(),opcion__.c_str());
        }
        else{
            this->imprimir_log(crear_cadena("Error, Parametro invalido no se reconoce > '%s'",opcion__.c_str()));
            return NULL;
        }

        this->resultado = PQexec(this->conexion, consulta_tmp.c_str());
        return this->comprueba_estado_consulta(PQresultStatus(this->resultado));
    }
    else{
        this->imprimir_log("Error, No se ha seleccionado una tabla para ejecutar esta funcion");
        return NULL;
    }
    return NULL;
}

///Metodos para saber la catidad de tuplas y de campos que se han obtenido en un consulta
int PGRecord::num_tuplas(){
    return (this->existe_con)?(PQntuples(this->resultado)):(0) ;
}

int PGRecord::num_campos(){
    return (this->existe_con)?(PQnfields(this->resultado)):(0) ;
}



char *PGRecord::comprueba_estado_consulta(ExecStatusType estado_consulta){

    if(estado_consulta == PGRES_EMPTY_QUERY ){
        this->liberar_resultado();
        this->imprimir_log("Se ha enviado una consulta vacia");
        return (char *)"{}";
    }
    else if( estado_consulta == PGRES_COMMAND_OK ){
        ///se ejecuta en las consultas de tipo INSERT, UPDATE que no devuelven datos
        this->liberar_resultado();
        this->imprimir_log("Consulta exitosa, No se consiguieron datos");
        return (char*)"{}";
    }
    else if( estado_consulta == PGRES_TUPLES_OK){
        this->imprimir_log(crear_cadena("Consulta exitosa, Tuplas: %d - Campos: %d ",this->num_tuplas(),this->num_campos()));
        this->liberar_resultado();
        return (char *)"{}";
    }
    else if( estado_consulta == PGRES_BAD_RESPONSE ){
        this->imprimir_log(crear_cadena("Error, Respuesta desconocida del servidor <> %s", PQresultErrorMessage(this->resultado)));
        this->liberar_resultado();
        return NULL;
    }
    else if( estado_consulta == PGRES_FATAL_ERROR){
        this->imprimir_log(crear_cadena("Error en la consulta <> %s",PQresultErrorMessage(this->resultado)));
        this->liberar_resultado();
        return NULL;
    }
    return NULL;
}


/****** FUNCIONES EXTERNAS *******/


///Funciones Auxiliares

inline char* crear_cadena(string  formato, ...){
	int tamano = 512;
	char* buffer = 0;
	buffer = new char[tamano];
	va_list lista;
	va_start(lista, formato);
	int nsize = vsnprintf(buffer, tamano, formato.c_str(), lista);
	if(tamano<=nsize){
		delete[] buffer;
		buffer = 0;
		buffer = new char[nsize+1]; //+1 for /0
		nsize = vsnprintf(buffer, tamano, formato.c_str(), lista);
	}
	string retorno(buffer);
	va_end(lista);
	delete[] buffer;
	return (char*)retorno.c_str();
}

char *crear_insert(string formato,va_list parametros){}
char *procesar_insertar(string formato){
}
