

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

enum object_type
{
    empty ,
    wall ,
    elf ,
    goblin
};

enum
{
    left,
    right,
    up,
    down
};


class Object
{
public:
    Object() {}
    Object( int x , int y , object_type type )
    {
        x_ = x ;
        y_ = y ;
        type_ = type ;
        hp_ = 200 ;
    }
    ~Object() {}

    int x() const { return x_ ; }
    int y() const { return y_ ; }
    object_type type() { return type_ ; }
    bool get_hit( int ap )
    {
        hp_ -= ap ;
        if ( hp_ < 0 )
            return true;
        return false ;
    }

    void move(int d) 
    {
        switch ( d )
        {
            case up:
                y_ -= 1;
                break;
            case down:
                y_ += 1;
                break;
            case left:
                x_ -= 1;
                break;
            case right:
                x_ += 1;
                break;
        }
    }

    int get_hp() { return hp_ ; }
private:
    int x_ , y_ ;
    int hp_ ;
    object_type type_ ;
};

/*class Node
{
    Node( Object * obj )
    {
        obj_ = obj;
        prev_ = nullptr;
        dist_ = std::numeric_limits<short>::max() ;
    }
    ~Node() {}
    void set_dist( short dist ) { dist_ = dist ;  }
    Object * get_obj() { return obj_ ; }
    Node * get_prev() { return prev_ ; }
    void set_prev( Node* prev ) { prev_ = prev ; }
private:
    Object * obj_ ;
    Node * prev_ ;
    short dist_ ;
};*/

bool object_comp ( Object * l ,  Object * r )
{
    if ( l->y() < r->y() ) return true ;
    else if ( l->y() == r->y() && l->x() < r->x() ) return true;
    else return false ;
}

class Grid
{
public:
    Grid() {}
    ~Grid() 
    {
        for ( auto it = grid_.begin() ; it != grid_.end() ; it++ )
        {
            delete (*it) ;
        }
    }


    Object * get_left( Object * current )
    {
        if ( current->x() == 0 ) return nullptr ;
        int pos = width_ * current->y() + current->x() ;
        return grid_[ pos - 1 ] ;
    }
    Object * get_up( Object * current )
    {
        if ( current->y() == 0 ) return nullptr ;
        int pos = width_ * current->y() + current->x() ;
        return grid_[ pos - width_ ] ;
    }
    Object * get_right( Object * current )
    {
        if ( current->x() == width_ - 1 ) return nullptr ;
        int pos = width_ * current->y() + current->x() ;
        return grid_[ pos + 1 ] ;
    }
    Object * get_down( Object * current )
    {
        if ( current->y() == height_ - 1 ) return nullptr ;
        int pos = width_ * current->y() + current->x() ;
        return grid_[ pos + width_ ] ;
    }

    void init( std::vector< std::string > lines ) 
    {
        height_ = lines.size() ;
        width_ = lines[ 0 ].size() ;
        for ( int i = 0 ; i < height_ ; i++ )
        {
            for ( int j = 0 ; j < width_ ; j++ )
            {
                switch ( lines[ i ][ j ] )
                {
                    case '#' :
                        grid_.push_back( new Object( j , i , wall ) ) ;
                        break ;
                    case 'G':
                        grid_.push_back( new Object( j , i , goblin ) ) ;
                        break ;
                    case 'E':
                        grid_.push_back( new Object( j , i , elf ) ) ;
                        break ;
                    case '.':
                        grid_.push_back( new Object( j , i , empty ) ) ;
                        break ;
                }
            }
        }
    }

    void print_grid()
    {
        for ( int i = 0 ; i < grid_.size() ; i++ )
        {
            if ( i % width_ == 0 ) std::cout << std::endl ;
            if ( grid_[ i ]->type() == empty ) std::cout << '.' ;
            else if ( grid_[ i ]->type() == wall ) std::cout << '#' ;
            else if ( grid_[ i ]->type() == elf ) std::cout << 'E' ;
            else if ( grid_[ i ]->type() == goblin ) std::cout << 'G' ;
        }
    }
    
    void get_players( std::vector< Object * > & players )
    {
        for ( auto it = grid_.begin() ; it != grid_.end() ; it++ )
        {
            if ( (*it)->type() == goblin || (*it)->type() == elf ) players.push_back( *it ) ;
        }
    }
    
    void get_targets( std::vector< Object * > & targets , std::vector< Object * > & players , Object * current )
    {
        for ( auto it = players.begin() ; it != players.end() ; it++ )
        {
            if ( ( *it ) != current && ( *it )->type() != current->type() )
                targets.push_back( *it ) ;
        }
    }

    void get_in_range( std::vector< Object * > & in_range , std::vector< Object * > & targets )
    {
        for ( auto it = targets.begin() ; it != targets.end() ; it++ )
        {
            //left
            if ( ( *it )->x() != 0 && grid_[ ( *it )->x() + ( *it )->y() * width_ - 1 ]->type() == empty )
                in_range.push_back( grid_[ ( *it )->x() + ( *it )->y() * width_ - 1 ] ) ;
            //right
            if ( ( *it )->x() != width_ - 1 && grid_[ ( *it )->x() + ( *it )->y() * width_ + 1 ]->type() == empty )
                in_range.push_back( grid_[ ( *it )->x() + ( *it )->y() * width_ + 1 ] ) ;
            // up
            if ( ( *it )->y() != 0 && grid_[ ( *it )->x() + ( ( *it )->y() -1 )* width_ ]->type() == empty )
                in_range.push_back( grid_[ ( *it )->x() + ( ( *it )->y() - 1 ) * width_ ] ) ;
            // down
            if ( ( *it )->y() != height_ - 1 && grid_[ ( *it )->x() + ( ( *it )->y() + 1 ) * width_ ]->type() == empty )
                in_range.push_back( grid_[ ( *it )->x() + ( ( *it )->y() + 1 ) * width_ ] ) ;
        }
        std::sort( in_range.begin() , in_range.end() , object_comp ) ;
    }

    int get_index( Object * o ) { return o->y() * width_ + o->x() ; }



    bool dijk( Object * start , Object * finish, short & dist , int & step )
    {
        std::vector< short > distances( grid_.size() , std::numeric_limits<short>::max() ) ;
        std::vector< int > previouses( grid_.size() , -1 ) ;
        distances[ get_index( start ) ] = 0 ;
        std::vector< Object * > unvisited ;
        for ( auto it = grid_.begin() ; it != grid_.end() ; it++ )
        {
            if ( ( *it )->type() == empty ) unvisited.push_back( *it ) ;
        }
        unvisited.push_back( start ) ;
        Object * current = start ;

        bool end = false ;
        while ( unvisited.size() > 0  )
        {
            Object * test = get_up( current ) ;
            if ( std::find( unvisited.begin() , unvisited.end() , test ) != unvisited.end() )
            {
                short new_dist = distances[ get_index( current ) ] + 1 ;
                distances[ get_index( test ) ] = std::min( new_dist , distances[ get_index( test ) ] ) ;
                previouses[ get_index( test ) ] = get_index( current ) ;
            }
            test = get_left( current ) ;
            if ( std::find( unvisited.begin() , unvisited.end() , test ) != unvisited.end() )
            {
                short new_dist = distances[ get_index( current ) ] + 1 ;
                distances[ get_index( test ) ] = std::min( new_dist , distances[ get_index( test ) ] ) ;
                previouses[ get_index( test ) ] = get_index( current ) ;
            }
            test = get_right( current ) ;
            if ( std::find( unvisited.begin() , unvisited.end() , test ) != unvisited.end() )
            {
                short new_dist = distances[ get_index( current ) ] + 1 ;
                distances[ get_index( test ) ] = std::min( new_dist , distances[ get_index( test ) ] ) ;
                previouses[ get_index( test ) ] = get_index( current ) ;
            }
            test = get_down( current ) ;
            if ( std::find( unvisited.begin() , unvisited.end() , test ) != unvisited.end() )
            {
                short new_dist = distances[ get_index( current ) ] + 1 ;
                distances[ get_index( test ) ] = std::min( new_dist , distances[ get_index( test ) ] ) ;
                previouses[ get_index( test ) ] = get_index( current ) ;
            }
            
            unvisited.erase( std::find( unvisited.begin() , unvisited.end() , current ) ) ;
            short next_dist = std::numeric_limits<short>::max() ;
            Object * next = nullptr ;
            for ( auto it = unvisited.begin() ; it != unvisited.end() ; it++ )
            {
                if ( distances[ get_index( *it ) ] < next_dist )
                {
                    next = *it ; next_dist = distances[ get_index( *it ) ] ;
                }
            }
            if ( next == nullptr ) return false ;
            
            current = next ;
            if ( current == finish )
            {
                int start_index = get_index( start ) ;
                int index = get_index( finish ) ;
                dist = distances[ index ] ;
                while ( previouses[ index ] != start_index )
                {
                    index = previouses[ index ] ;
                }
                int offset = index - start_index ;
                step = offset == 1 ? right :
                    offset == -1 ? left :
                    offset == width_ ? down :
                    offset == -width_ ? up : -1 ;
                return true ;
            }
        }
        return false ;
    }

    int get_selected( std::vector< Object * > & in_range , Object * start )
    {
        std::vector< Object * > reachable ;
        std::vector< short > dists ;
        std::vector< int > steps ;
        for ( auto it = in_range.begin() ; it != in_range.end() ; it++ )
        {
            short dist = 0;
            int step = -1;
            if ( dijk( start , *it , dist , step ) )
            {
                reachable.push_back( *it );
                dists.push_back( dist );
                steps.push_back( step ) ;
            }
        }
        if ( reachable.size() == 0 ) return -1;
        auto min = std::min_element( dists.begin() , dists.end() ) ;
        int min_val = *min ;
        for ( int i = 0; i < dists.size() ; i++ )
        {
            if ( dists[ i ] >  min_val )
            {
                dists.erase( dists.begin() + i );
                reachable.erase( reachable.begin() + i ) ;
                steps.erase( steps.begin() + i ) ;
            }
        }

        return steps[ 0 ] ;
    }

    void move( Object * o , int d )
    {
        switch ( d )
        {
            case up:
                get_up( o )->move( down );
                o->move( up );
                break;
            case down:
                get_down( o )->move( up );
                o->move( down );
                break;
            case left:
                get_left( o )->move( right );
                o->move( left );
                break;
            case right:
                get_right( o )->move( left );
                o->move( right );
                break;
        }
        std::sort( grid_.begin() , grid_.end() , object_comp ) ;
    }

    Object * get_attack_target( Object * o )
    {
        std::vector< Object * > targets;

        Object * up = get_up( o );
        Object * down = get_down( o );
        Object * left = get_left( o );
        Object * right = get_right( o );

        if ( ( up->type() == goblin ||
               up->type() == elf   ) &&
               up->type() != o->type() ) 
             targets.push_back( up ) ;

        if ( ( down->type() == goblin ||
             down->type() == elf ) &&
             down->type() != o->type() )
            targets.push_back( down ) ;

        if ( ( left->type() == goblin ||
             left->type() == elf ) &&
             left->type() != o->type() )
            targets.push_back( left ) ;

        if ( ( right->type() == goblin ||
             right->type() == elf ) &&
             right->type() != o->type() )
            targets.push_back( right ) ;

        Object * target = nullptr ;
        int hp = 201;
        for ( auto it = targets.begin() ; it != targets.end() ; it++ )
        {
            if ( ( *it )->get_hp() < hp )
            {
                hp = (*it)->get_hp() ;
            }
        }
        std::remove_if( targets.begin() , targets.end() , [ hp ] ( Object * o ) { return o->get_hp() > hp; } ) ;
        if ( targets.size() > 1 )
            std::sort( targets.begin() , targets.end() , object_comp ) ;
        return targets[ 0 ] ;
    }

    bool round()
    {
        std::vector< Object * > players;
        get_players( players ) ;
        for ( auto it = players.begin() ; it != players.end() ; it++ )
        {
            std::vector< Object * > targets;
            get_targets( targets , players , *it ) ;
            if ( targets.size() == 0 ) return false;
            // check if adjacent target

            bool adjacent = false ;
            if ( std::find( targets.begin() , targets.end() , get_up( *it ) ) !=  targets.end() ) 
                 adjacent = true ;
            if ( std::find( targets.begin() , targets.end() , get_left( *it ) ) !=  targets.end() ) 
                adjacent = true ;
            if ( std::find( targets.begin() , targets.end() , get_right( *it ) ) !=  targets.end() ) 
                adjacent = true ;
            if ( std::find( targets.begin() , targets.end() , get_down( *it ) ) !=  targets.end() ) 
                adjacent = true ;

            if ( !adjacent )
            {
                std::vector< Object * > in_range;
                get_in_range( in_range , targets ) ;
                if ( in_range.size() == 0 ) continue ;
                int step = get_selected( in_range , *it ) ;
                if ( step == -1 ) continue ;
                move( *it , step ) ;
                //print_grid() ;

                if ( std::find( targets.begin() , targets.end() , get_up( *it ) ) != targets.end() )
                    adjacent = true ;
                if ( std::find( targets.begin() , targets.end() , get_left( *it ) ) != targets.end() )
                    adjacent = true ;
                if ( std::find( targets.begin() , targets.end() , get_right( *it ) ) != targets.end() )
                    adjacent = true ;
                if ( std::find( targets.begin() , targets.end() , get_down( *it ) ) != targets.end() )
                    adjacent = true ;
            }
            if ( adjacent )
            {
                Object * target = get_attack_target( *it ) ;
                if ( target->get_hit( 3 ) )
                {
                    grid_[ get_index( target ) ] = new Object( target->x() , target->y() , empty ) ;
                    delete target ;
                }
               // std::cout << "attack!" << std::endl;
            }
        }
        return true ;
    }



private:
    int width_ , height_ ;
    std::vector< Object * > grid_ ;
};

int main()
{
    std::vector< std::string > lines ;
    lines.push_back( "#######" );
    lines.push_back( "#.G...#" );
    lines.push_back( "#...EG#" );
    lines.push_back( "#.#.#G#" );
    lines.push_back( "#..G#E#" );
    lines.push_back( "#.....#" );
    lines.push_back( "#######" );

    /*lines.push_back( "#######" );
    lines.push_back( "#E..G.#" );
    lines.push_back( "#...#.#" );
    lines.push_back( "#.G.#G#" );
    lines.push_back( "#######" );*/

    Grid grid;
    grid.init( lines ) ;
    grid.print_grid() ;
    
    bool cont = true ;
    while ( cont )
    {
        cont = grid.round() ;
        grid.print_grid() ;
    }

    char q ;
    std::cin >> q ;
}

