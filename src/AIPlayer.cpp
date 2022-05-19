#include "AIPlayer.h"
#include "Parchis.h"

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
        Poda_AlfaBeta(*actual, jugador, c_piece, id_piece, dice);
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

void AIPlayer::Poda_AlfaBeta(Parchis actual, int jugador, color &c_piece, int &id_piece, int &dice) const
{
    vector<int> current_dices;
    vector<int> current_pieces;

    c_piece = actual.getCurrentColor();

    current_dices = actual.getAvailableDices(c_piece);

    int bestScore = -INFINITY;
    int id_piece_candidate = -1;
    int dice_candidate = -1;

    for (int i = 0; i < current_dices.size(); i++)
    {

        // Se obtiene el vector de fichas que se pueden mover para el dado elegido
        current_pieces = actual.getAvailablePieces(c_piece, current_dices[i]);

        for (int j = 0; j < current_pieces.size(); j++)
        {
            c_piece = actual.getCurrentColor();
            actual.movePiece(c_piece, current_pieces[j], current_dices[i]);

            int score = minimax(actual, jugador, 1, 0, c_piece, id_piece, dice, -10000, +10000);
            if (bestScore < score)
            {
                bestScore = score;
                id_piece_candidate = current_pieces[j];
                dice_candidate = current_dices[i];
            }
        }
    }

    if (id_piece_candidate == -1)
    {
        id_piece = SKIP_TURN;
    }
    else
    {
        id_piece = id_piece_candidate;
        dice = dice_candidate;
    }
}

int AIPlayer::minimax(Parchis actual, int jugador, int maximizing, int profundidad, color &c_piece, int &id_piece, int &dice, int alpha, int beta) const
{

    if (profundidad == PROFUNDIDAD_ALFABETA)
    {
        // Aqui implementaremos la heuristica
        return ValoracionTest(actual, maximizing);
    }

    if (maximizing)
    {

        vector<int> current_dices;
        vector<int> current_pieces;

        c_piece = actual.getCurrentColor();

        current_dices = actual.getAvailableDices(c_piece);

        vector<int> current_dices_que_pueden_mover_ficha;
        for (int i = 0; i < current_dices.size(); i++)
        {

            // Se obtiene el vector de fichas que se pueden mover para el dado elegido
            current_pieces = actual.getAvailablePieces(c_piece, current_dices[i]);

            for (int j = 0; j < current_pieces.size(); j++)
            {

                c_piece = actual.getCurrentColor();
                actual.movePiece(c_piece, current_pieces[j], current_dices[i]);
                actual.de
                int maybeAlpha = minimax(actual, jugador, 0, profundidad + 1, c_piece, id_piece, dice, alpha, beta);
                alpha = max(maybeAlpha, alpha);

                if (alpha >= beta)
                {
                    break;
                }
            }
        }

        return alpha;
    }
    else
    {

        vector<int> current_dices;
        vector<int> current_pieces;

        c_piece = actual.getCurrentColor();

        current_dices = actual.getAvailableDices(c_piece);

        vector<int> current_dices_que_pueden_mover_ficha;
        for (int i = 0; i < current_dices.size(); i++)
        {

            // Se obtiene el vector de fichas que se pueden mover para el dado elegido
            current_pieces = actual.getAvailablePieces(c_piece, current_dices[i]);

            for (int j = 0; j < current_pieces.size(); j++)
            {

                c_piece = actual.getCurrentColor();
                actual.movePiece(c_piece, current_pieces[j], current_dices[i]);

                int maybeBeta = minimax(actual, jugador, 1, profundidad + 1, c_piece, id_piece, dice, alpha, beta);
                beta = min(maybeBeta, beta);

                if (alpha >= beta)
                {
                    break;
                }
            }
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
                else if (estado.getBoard().getPiece(c, j).type == home)
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
                else if (estado.getBoard().getPiece(c, j).type == home)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}
