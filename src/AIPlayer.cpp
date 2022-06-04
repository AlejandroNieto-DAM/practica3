#include "AIPlayer.h"
#include "Parchis.h"
#include "algorithm"
#include <thread>
#include <chrono>


const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;
const int num_pieces = 4;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta

bool AIPlayer::move()
{
    cout << "Realizo un movimiento automatico" << endl;

    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}

void AIPlayer::think(color &c_piece, int &id_piece, int &dice) const
{

    switch (id)
    {
    case 0:
        thinkAleatorio(c_piece, id_piece, dice);
        break;
    case 1:
        thinkAleatorioMasInteligente(c_piece, id_piece, dice);
        break;
    case 2:
        thinkFichaMasAdelantada(c_piece, id_piece, dice);
        break;
    case 3:
        thinkMejorOpcion(c_piece, id_piece, dice);
        break;
    case 4:
        Poda_AlfaBeta(actual, actual->getCurrentPlayerId(), PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, menosinf, masinf);
        if(id_piece > 4){
            id_piece = SKIP_TURN;
        }
        break;
    }


    /*
    // El siguiente código se proporciona como sugerencia para iniciar la implementación del agente.

    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta
    // Llamada a la función para la poda (los parámetros son solo una sugerencia, se pueden modificar).
    valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    // ----------------------------------------------------------------- //

    // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch(id){
        case 0:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
            break;
        case 1:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion1);
            break;
        case 2:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion2);
            break;
    }
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    */
}

bool thereIsOpponentPiece(Parchis estado, int jugador, int casilla){
    int oponente = (jugador + 1) % 2;
    vector<color> op_colors = estado.getPlayerColors(oponente);

    int i = 0;
    bool exist = false;

    for (int i = 0; i < op_colors.size() and exist == false; i++)
    {
        color c = op_colors[i];
        // Recorro las fichas de ese color.
        for (int j = 0; j < num_pieces; j++)
        {
            // Valoro positivamente que la ficha esté en casilla segura o meta.
            if (estado.getBoard().getPiece(c, j).num == casilla)
            {
                i += 1;
            }

        }

        if(i != 2){
            exist = true;
        }

    }

    return exist;

}

double AIPlayer::Heuristica(Parchis &estado, int jugador) const {
    int ganador = estado.getWinner();
    int oponente = (jugador + 1) % 2;

      
    // Recorro todas las fichas de mi jugador
    int puntuacion_jugador = 0;
    int puntuacion_oponente = 0;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    } 
    else 
    {

        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        int sum = 0;


        // Recorro colores de mi jugador.
        pair<int, int> piezasEnCasa(0, 0);
        
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            vector<int> avaliableDices = estado.getAvailableDices(my_colors[i]);
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {

                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;
                }
                else if (estado.getBoard().getPiece(c, j).type == goal)
                {
                    if(i == 0)
                        puntuacion_jugador += 10;
                    else
                        puntuacion_jugador += 5;
                }
                else if (estado.getBoard().getPiece(c, j).type == final_queue)
                {
                    puntuacion_jugador += 5;
                }  
                else if (estado.getBoard().getPiece(c, j).type == home)
                {

                    if(i == 0){
                        piezasEnCasa.first += 1;
                    } else {
                        piezasEnCasa.second += 1;
                    }

                    puntuacion_jugador -= 1;
                } 
                else if(estado.isWall(estado.getBoard().getPiece(c,j))){
                    puntuacion_jugador += 2;
                }
                sum += estado.distanceToGoal(c, j);
            }
        }

        puntuacion_jugador -= sum/10;

        vector<tuple <color, int, Box, Box>> aux;
        /*for(int i = 0; i < aux.size(); i++){
            if(get<0>(aux[i]) == my_colors[0] || get<0>(aux[i]) == my_colors[1]){
                puntuacion_jugador += 1;
            } else if(get<0>(aux[i]) == op_colors[0] || get<0>(aux[i]) == op_colors[1]){
                puntuacion_oponente += 1;
            }
        }*/

        if(estado.isEatingMove()){
            if(estado.getCurrentPlayerId() == jugador){
                puntuacion_jugador += 20;
                
            } else  {
                puntuacion_oponente += 20;
            }
        }


        
        sum = 0;
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {

                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).type == goal)
                {
                    puntuacion_oponente += 10;
                }
                else if (estado.getBoard().getPiece(c, j).type == final_queue)
                {
                    puntuacion_oponente += 5;
                }  
                else if (estado.getBoard().getPiece(c, j).type == home)
                {
                    puntuacion_oponente -= 1;
                } 
                else if(estado.isWall(estado.getBoard().getPiece(c,j))){
                    puntuacion_oponente += 2;
                }

                sum += estado.distanceToGoal(c, j);

            }

        }

        puntuacion_oponente -= sum/10;


        


        

        /* Vamos a ver donde tenemos las casillas y si las tenemos en una safe place vamos a sumarle puntos */
        /* Tambien vamos a ver los dados que nos quedan y si alguno es bueno para llegar a meta sumamos puntos
        aun asi debemos tener en cuenta que si estamos en casa y no tenemos mas piezas sacadas seria tonteria */
        /* Tenemos que ver si nos quedan dados con los que podamos comernos a una ficha y antes de hacerlo tener una ficha 
        sacada */
        /* Hacer que el amarillo se coma al rojo (Un mismo equipo que use su otro color para boostear) */
        /* Mirar las fichas enemigas que esten a menos de 7 */
        /* Mirar las fichas enemigas que esten detras mia a menos de 7*/
        /* Mirar si a menos de 7 hacia delante hay una barrera*/
        /* Si la ficha esta a 13 de la meta o 9 esta muy bien pero a 9  mejor (ya esta 100% asegurada) */

    }

    return puntuacion_jugador - puntuacion_oponente;

} 

double AIPlayer::Poda_AlfaBeta(Parchis * actual, int jugador, int profundidad, color & c_piece, int & id_piece, int &dice, double  alpha, double  beta) const
{

    if (profundidad == 0 || actual->gameOver())
    {
        return Heuristica(*actual, jugador);

    }

    //cout << c_piece << " " << id_piece << " " << dice << " " << profundidad << " " << alpha << " " << beta << endl;

    if(actual->getCurrentPlayerId() == jugador){

        int last_id_piece = -1;
        int last_dice = -1;
        color last_c_piece = none;

        Parchis siguiente_hijo = actual->generateNextMoveDescending(last_c_piece, last_id_piece, last_dice);

        double value = menosinf;

        while(!(siguiente_hijo == *actual)){

            value = max(alpha, Poda_AlfaBeta(&siguiente_hijo, jugador, profundidad - 1, last_c_piece, last_id_piece, last_dice, alpha, beta));

            if(value > alpha){
                alpha = value;

                if(profundidad == PROFUNDIDAD_ALFABETA){
                    c_piece = last_c_piece;
                    id_piece = last_id_piece;
                    dice = last_dice;
                }
                
            }
            
            if (beta <= alpha)
            {
                break;
            }

            siguiente_hijo = actual->generateNextMoveDescending(last_c_piece, last_id_piece, last_dice);

        }

        return alpha;

    } else {

        int last_id_piece = -1;
        int last_dice = -1;
        color last_c_piece = none;

        Parchis siguiente_hijo = actual->generateNextMoveDescending(last_c_piece, last_id_piece, last_dice);
            
        double value = masinf;

        while(!(siguiente_hijo == *actual)){
            
            value = min(beta, Poda_AlfaBeta(&siguiente_hijo, jugador, profundidad - 1, last_c_piece, last_id_piece, last_dice, alpha, beta));

            if(value < beta){
                beta = value;

                if(profundidad == PROFUNDIDAD_ALFABETA){
                    c_piece = last_c_piece;
                    id_piece = last_id_piece;
                    dice = last_dice;
                }
            }

            if (beta <= alpha)
            {
                break;

            } 

            siguiente_hijo = actual->generateNextMoveDescending(last_c_piece, last_id_piece, last_dice);
            
        }

        return beta;
    }

}

void AIPlayer::thinkAleatorio(color &c_piece, int &id_piece, int &dice) const
{
    // IMPLEMENTACIÓN INICIAL DEL AGENTE
    // Esta implementación realiza un movimiento aleatorio.
    // Se proporciona como ejemplo, pero se debe cambiar por una que realice un movimiento inteligente
    // como lo que se muestran al final de la función.

    // OBJETIVO: Asignar a las variables c_piece, id_piece, dice (pasadas por referencia) los valores,
    // respectivamente, de:
    // - color de ficha a mover
    // - identificador de la ficha que se va a mover
    // - valor del dado con el que se va a mover la ficha.

    // El color de ficha que se va a mover
    c_piece = actual->getCurrentColor();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<int> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableDices(c_piece);
    // Elijo un dado de forma aleatoria.
    dice = current_dices[rand() % current_dices.size()];

    // Se obtiene el vector de fichas que se pueden mover para el dado elegido
    current_pieces = actual->getAvailablePieces(c_piece, dice);

    // Si tengo fichas para el dado elegido muevo una al azar.
    if (current_pieces.size() > 0)
    {
        id_piece = current_pieces[rand() % current_pieces.size()];
    }
    else
    {
        // Si no tengo fichas para el dado elegido, pasa turno (la macro SKIP_TURN me permite no mover).
        id_piece = SKIP_TURN;
    }
}

void AIPlayer::thinkAleatorioMasInteligente(color &c_piece, int &id_piece, int &dice) const
{
    // El color de ficha que se va a mover
    c_piece = actual->getCurrentColor();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<int> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableDices(c_piece);

    // En vez de elegir un dado al azar, miro primero cuales tienen fichas que se pueden mover
    vector<int> current_dices_que_pueden_mover_ficha;
    for (int i = 0; i < current_dices.size(); i++)
    {

        // Se obtiene el vector de fichas que se pueden mover para el dado elegido
        current_pieces = actual->getAvailablePieces(c_piece, current_dices[i]);

        // Si se pueden mover fichas para el dado actual, lo añado al vector de dados que pueden mover ficha
        if (current_pieces.size() > 0)
        {
            current_dices_que_pueden_mover_ficha.push_back(current_dices[i]);
        }
    }

    // Si no tengo ningun dado que pueda mover ficha, paso turno tirando un dado al azar
    if (current_dices_que_pueden_mover_ficha.size() == 0)
    {
        dice = current_dices[rand() % current_dices.size()];

        id_piece = SKIP_TURN;
    }
    else
    {
        // En caso contrario, elijo un dado de forma aleatoria de entre los que pueden mover ficha
        dice = current_dices_que_pueden_mover_ficha[rand() % current_dices_que_pueden_mover_ficha.size()];

        // Se obtiene el vector de fichas que se pueden mover para el dado elegido
        current_pieces = actual->getAvailablePieces(c_piece, dice);

        // Muevo una ficha al azar entre las que se puedan mover
        id_piece = current_pieces[rand() % current_pieces.size()];
    }
}

void AIPlayer::thinkFichaMasAdelantada(color &c_piece, int &id_piece, int &dice) const
{
    // Elijo el dado haciendo lo mismo que el jugador anterior
    thinkAleatorioMasInteligente(c_piece, id_piece, dice);
    // Tras llamar a esta funcion ya tengo en el dado el numero de dado que quiero usar.
    // Ahora en vez de mover una ficha al azar voy a mover la que este mas adelantada
    //(la mas cercana a la meta)

    vector<int> current_pieces = actual->getAvailablePieces(c_piece, dice);

    int id_ficha_mas_adelantada = -1;
    int min_distancia_meta = 9999;
    for (int i = 0; i < current_pieces.size(); i++)
    {
        int distancia_meta = actual->distanceToGoal(c_piece, current_pieces[i]);
        if (distancia_meta < min_distancia_meta)
        {
            min_distancia_meta = min_distancia_meta;
            id_ficha_mas_adelantada = current_pieces[i];
        }
    }

    if (id_ficha_mas_adelantada == -1)
    {
        id_piece = SKIP_TURN;
    }
    else
    {
        id_piece = id_ficha_mas_adelantada;
    }
}

void AIPlayer::thinkMejorOpcion(color &c_piece, int &id_piece, int &dice) const
{
    int last_id_piece = -1;
    int last_dice = -1;
    color last_c_piece = none;

    Parchis siguiente_hijo = actual->generateNextMove(last_c_piece, last_id_piece, last_dice);

    bool me_quedo_con_esta_accion = false;

    while(!(siguiente_hijo == *actual) and !me_quedo_con_esta_accion){
        if(siguiente_hijo.isEatingMove() or siguiente_hijo.isGoalMove() or
        (siguiente_hijo.gameOver() and siguiente_hijo.getWinner() == this->jugador)){
            me_quedo_con_esta_accion = true;
        } else {
            siguiente_hijo = actual->generateNextMove(last_c_piece, last_id_piece, last_dice);
        }
    }

    if(me_quedo_con_esta_accion){
        c_piece = last_c_piece;
        id_piece = last_id_piece;
        dice = last_dice;
    } else {
        thinkFichaMasAdelantada(c_piece, id_piece, dice);
    }


}

double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.

    int ganador = estado.getWinner();
    int oponente = (jugador + 1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;
                }
                else if (estado.getBoard().getPiece(c, j).type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}
