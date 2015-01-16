/**
    Creado por: Israel Lugo
    Fecha: 27 Diciembre 2014
    Bibliotecas auxiliares:
        libpq-fe.h (PosgrestSQL)
        stdio.h
        string.h
        stdlib.h
        stdarg.h
        iostream.h
        map.h
        ctype.h
            Descripcion:
        Biblioteca hecha con el fin de facilitar la gestion de conexiones y consultas al motor de base de datos postgres
        esta basado en la idea de ActiveRecord.. Lenguaje c++

**/


#ifndef __PGRECORD_H__
#define __PGRECORD_H__

#include <postgresql/libpq-fe.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <map>
#include <ctype.h>

///Constantes simbolicas

#define CONEXION_EXITOSA 0
#define CONEXION_FALLIDA 1


///Tipos de datos pre-configurados
using namespace std;

typedef map<string, int> Tipo_opcion;
typedef Tipo_opcion::value_type crea_opcion;

class PGRecord {

    private:
        ///Datos de conexion
        string host, db, usuario, password, puerto, opciones, tty;

        ///Variables para el manejo de postgres
        PGconn *conexion;
        PGresult *resultado;

        ///Variable para el uso de una tabla en especifico
        string tabla_en_uso;

        ///Variable para saber si existe una conexion con la base de datos
        bool existe_con;

        ///Variable para saber cuando imprimir el log de las operaciones
        bool mostrar_log;
        void imprimir_log(const string);

        ///Funciones auxiliares de la clase
        char *comprueba_estado_consulta(ExecStatusType estado_consulta);


    public:
        ///Constructores
        PGRecord();
        PGRecord(string host, string puerto, string db, string usuario, string password);
        PGRecord(string host, string puerto, string opciones, string tty,string db, string usuario, string password);
        PGRecord(string datos_conexion);

        ///Destructor
        ~PGRecord();

        ///Metodos set >parametros de conexion<
        void set_host(string _host);
        void set_puerto(string _puerto);
        void set_opciones(string _opciones);
        void set_tty(string _tty);
        void set_db(string _db);
        void set_usuario(string _usuario);
        void set_password(string _password);
        void set_datos_conexion(string _host, string _puerto, string _db, string _usuario, string _password);

        ///Metodos get >parametros de conexion<
        char *get_host();
        char *get_puerto();
        char *get_opciones();
        char *get_tty();
        char *get_db();
        char *get_usuario();
        char *get_password();
        char *get_datos_conexion();

        ///Metodo para mostrar mensajes arrojados por los metodos
        void log(bool _estado);

        ///Metodo para conectar con la base de datos
        ///@Retorno True si la conexion fue exitosa, False si no se conecto
        bool conectar();
        void cerrar_conexion();
        void reiniciar_conexion();
        void liberar_resultado();

        ///@Retorno int Devuelve el ID del proceso que mantiene la conexion
        int get_id_proceso();

        ///Metodo para saber el estado de la conexion
        ///@Retorno int
        int estado();

        ///Metodo para consultar a la base de datos
        ///@Retorno char *
        char *consulta(string query);

        ///Metodos para la interaccion directa con una tabla
        ///Los metodos seleccionar, insertar, actualizar, eliminar..  Solamente funcionaran si se ha seleccionado una tabla por defecto
        void usar_tabla(string nombre_tabla);
        ///Selecciona Todos los campos de la tabla seleccionada, solo necesita la cantidad de registros que devolvera
        char *seleccionar(string cantidad = "*");
        ///Selecciona los campos especificados de la tabla seleccionada, necesita la cantidad de registros que devolvera
        char *seleccionar(string campos, string cantidad);
        void *insertar(string formato, ...);
        void *actualizar();
        void *eliminar();
        ///Metodos para saber la catidad de tuplas y de campos que se han obtenido en un consulta
        ///@Retorno int
        int num_tuplas();
        int num_campos();

        ///Metodo para obtener el nombre de cada campo
        char *obtener_nombre_campos();
        char *obtener_nombre_campos(string tabla);
};


///Funciones auxiliares
//char *crear_cadena(string formato, int tamano_total, ...);
//void convertir_cadena(string origen, string destino,string tipo);
inline char *crear_cadena(string  formato, ...);

char *crear_insert(string formato,va_list parametros);
char *procesar_insertar(string formato);



#endif // __PGRECORD_H__

