<?php
//$Id$

//------------------------------------------------------------------------------
// Database manipulation functions
//------------------------------------------------------------------------------

function uc_bulk_discount_select_rules( $conditions, $force_array = FALSE ) {
  
  $where      = array();
  $where_args = array();
  
  if ( is_array( $conditions ) && count( $conditions ) ) {
    foreach ( $conditions as $key => $value ) {
      switch ( $key ) {
      
        case 'pdid':
        case 'nid':      
        case 'xid':
        case 'low_qty':
        case 'high_qty':
          $where[] = "$key = %d";
          break;
          
        case 'discount':
        	$where[] = "$key = %f";
        	break;
        
        case 'type':        
          $where[] = "$key = '%s'";        
          break;

        default:
          continue;
      }
    
      $where_args[] = $value;
    }
  }
  
  $discount_rules = array();
  
  $results = db_query( 'SELECT * FROM {uc_bulk_discounts}' 
                       . ( count( $where ) ? ' WHERE ' . join( ' AND ', $where ) 
                                           : '' ),
                       $where_args );
                       
  while ( $rule = db_fetch_array( $results ) ) {
    $discount_rules[] = $rule;  
  }
  
  if ( ! $force_array && count( $discount_rules ) <= 1 ) {
    return ( count( $discount_rules ) ? $discount_rules[ 0 ] : FALSE );
  }
  
  usort( $discount_rules, '_uc_bulk_discount_sort_rules' );
   
  return $discount_rules;  
}

//------------------------------------------------------------------------------

function uc_bulk_discount_add_rule( $nid, $type, $xid, 
																			$low_qty, $high_qty, $discount, 
                                      $skip_check = FALSE ) {  
  if ( ! $skip_check ) {
    $discount_rule = uc_bulk_discount_select_rules( 
      array( 
  			'nid' => $nid, 
    		'type' => $type,
    		'xid' => $xid 
      )
    );
  
    if ( $discount_rule ) {
      $discount_rule[ 'low_qty' ]  = $low_qty;
      $discount_rule[ 'high_qty' ] = $high_qty;
      $discount_rule[ 'discount' ] = $discount;
        
      return uc_bulk_discount_save_rule( $discount_rule, TRUE );
    }
  }
  
  $discount_rule = array(
    'nid'   => $nid,
    'type'  => $type,
    'xid'   => $xid,
  	'low_qty' => $low_qty,
  	'high_qty' => $high_qty,
    'discount' => $discount
  );
   
  db_query( 
  	'INSERT INTO {uc_bulk_discounts}'
  	. ' ( nid, type, xid, low_qty, high_qty, discount )'
  	. ' VALUES ( %d, "%s", %d, %d, %d, %f )',
  	$discount_rule[ 'nid' ], 
  	$discount_rule[ 'type' ], 
  	$discount_rule[ 'xid' ], 
  	$discount_rule[ 'low_qty' ],
    $discount_rule[ 'high_qty' ],
    $discount_rule[ 'discount' ]
  );
  
  $discount_rule[ 'pdid' ] = db_last_insert_id( 'uc_bulk_discounts', 'pdid' );
  
  return $discount_rule;  
}

//------------------------------------------------------------------------------

function uc_bulk_discount_save_rule( $discount_rule, $skip_check = FALSE ) {
  
  if ( ! $skip_check ) {
    if ( $discount_rule[ 'pdid' ] ) {
      $existing_rule = uc_bulk_discount_select_rules( 
        array( 
    			'pdid' => $discount_rule[ 'pdid' ] 
        ) 
      );
    }
    else {
      $is_new = TRUE;
    }
  
    if ( $is_new || ! $existing_rule ) {
      return uc_bulk_discount_add_rule(
        $discount_rule[ 'nid'],
        $discount_rule[ 'type' ],
        $discount_rule[ 'xid' ],
        $discount_rule[ 'low_qty' ],
        $discount_rule[ 'high_qty' ],
        $discount_rule[ 'discount' ],
        TRUE 
      );
    }
  }
  
  db_query( 'UPDATE {uc_bulk_discounts}'
  					. ' SET nid = %d, type = "%s", xid = %d,'
  					. ' low_qty = %d, high_qty = %d, discount = %f'
  					. ' WHERE pdid = %d',
    $discount_rule[ 'nid' ],
    $discount_rule[ 'type' ],
    $discount_rule[ 'xid' ],
    $discount_rule[ 'low_qty' ],
    $discount_rule[ 'high_qty' ],
    $discount_rule[ 'discount' ],
    $discount_rule[ 'pdid' ] 
  );
  
  return $discount_rule;
}

//------------------------------------------------------------------------------

function uc_bulk_discount_remove_rule( $discount_rule ) {
  
  if ( is_array( $discount_rule ) ) {
    $pdid = $discount_rule[ 'pdid' ];
  }
  else {
    $pdid = $discount_rule;
  }
  
  db_query( 'DELETE FROM {uc_bulk_discounts} WHERE pdid = %d', $pdid );
}

//------------------------------------------------------------------------------

function _uc_bulk_discount_sort_rules( $rule1, $rule2 ) {
  
  // First sort by node id.
  if ( $rule1[ 'nid' ] != $rule2[ 'nid' ] ) {
    return ( $rule1[ 'nid' ] < $rule2[ 'nid' ] ? -1 : 1 );   
  }  
  // Then sort by type.
  if ( $rule1[ 'type' ] != $rule2[ 'type' ] ) {
    return ( $rule1[ 'type' ] == UCBD_TYPE_ROLE ? -1 : 1 );
  }
  // Then sort by xid.
  if ( $rule1[ 'xid' ] != $rule2[ 'xid' ] ) {
    return ( $rule1[ 'xid' ] < $rule2[ 'xid' ] ? -1 : 1 );  
  }
  // Then sort by lowest quantity.
  if ( $rule1[ 'low_qty' ] != $rule2[ 'low_qty' ] ) {
  	return ( $rule1[ 'low_qty' ] < $rule2[ 'low_qty' ] ? -1 : 1 );
  }
  // They are practically the same.
  return 0;
}

//------------------------------------------------------------------------------

