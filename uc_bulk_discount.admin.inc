<?php
//$Id$

//------------------------------------------------------------------------------
// Menu callbacks
//------------------------------------------------------------------------------

function uc_bulk_discount_list_discounts( $node ) {
  module_load_include( 'inc', 'uc_bulk_discount', 'uc_bulk_discount.api' );
              
  $discount_rules = uc_bulk_discount_select_rules( 
    array( 'nid' => $node->nid ), TRUE );
    
  return theme( 'uc_bulk_discount_rule_listing', $node, $discount_rules );
}

//------------------------------------------------------------------------------

function uc_bulk_discount_add_price_form( $form_state, $node ) {
  module_load_include( 'inc', 'uc_bulk_discount', 'uc_bulk_discount.api' );  
  return uc_bulk_discount_rule_form( $node, NULL );
}

//------------------------------------------------------------------------------

function uc_bulk_discount_edit_price_form( $form_state, $node, $discount_rule ) {
  module_load_include( 'inc', 'uc_bulk_discount', 'uc_bulk_discount.api' );  
  return uc_bulk_discount_rule_form( $node, $discount_rule );
}


//------------------------------------------------------------------------------

function uc_bulk_discount_delete_price_form( $form_state, $node, $discount_rule ) {
  module_load_include( 'inc', 'uc_bulk_discount', 'uc_bulk_discount.api' );
  
  $form[ 'nid' ] = array( 
  	'#type' => 'hidden', 
  	'#value' => $node->nid,
  );
  $form[ 'pdid'] = array(
    '#type' => 'hidden',
    '#value' => $discount_rule[ 'pdid' ],
  );
  
  $form[ '#submit' ][ ] = 'uc_bulk_discount_rule_delete_confirm_submit';

  return confirm_form( $form,
    t( 'Are you sure you want to delete this rule?' ),
		"node/$node->nid/edit/discounts", 
    t( 'This action cannot be undone.' ),
    t( 'Delete' ), 
    t( 'Cancel' ) 
  );
}

//------------------------------------------------------------------------------
// Form handlers
//------------------------------------------------------------------------------

function uc_bulk_discount_rule_form( $node, $discount_rule ) {
  
  $form = array();
  
  // Form inputs
  // 1. type [ radio buttons ] - type
  //  * username [ textfield / autocomplete ] - uid
  //  * role name [ select box ] - rid
  // 2. low quantity [ textfield ] - integer
  // 3. high quantity [ textfield ] - integer
  // 2. discount [ textfield ] - integer
  
  drupal_add_js( drupal_get_path( 'module', 'uc_bulk_discount' ) . '/uc_bulk_discount.js' );
  
  $form[ '#node' ]          = $node;
  $form[ '#discount_rule' ] = $discount_rule;
  
  $form[ 'rule_type' ] = array(
  	'#type' => 'radios',
    '#id' => 'rule_type',
  	'#title' => t( 'Discount rule type' ),
  	'#default_value' => ( 
      $discount_rule && $discount_rule[ 'type' ] == UCBD_TYPE_ROLE ? 1 : 0 
    ),
  	'#options' => array( t( 'User' ), t( 'Role' ) ),
    '#weight' => -10,
  );
  
  $form[ 'min_qty' ] = array(
  	'#type' => 'textfield',
  	'#id' => 'min_qty',
  	'#title' => t( 'Minimum quantity for this discount' ),
  	'#maxlength' => 60,
  	'#default_value' => ( $discount_rule ? $discount_rule[ 'min_qty' ] : '' ),
  	'#weight' => 8,
  	'#required' => TRUE,
  	'#element_validate' => array( 'uc_bulk_discount_validate_integer' ),
  );
  
  $form[ 'max_qty' ] = array(
  	'#type' => 'textfield',
  	'#id' => 'max_qty',
  	'#title' => t( 'Maximum quantity for this discount' ),
  	'#maxlength' => 60,
  	'#default_value' => ( $discount_rule ? $discount_rule[ 'max_qty' ] : '' ),
  	'#weight' => 9,
  	'#required' => TRUE,
  	'#element_validate' => array( 'uc_bulk_discount_validate_integer' ),
  );
  
  $form['discount'] = array(
    '#type' => 'textfield',
  	'#id' => 'rule_discount',
    '#title' => t('Percentage discount'),
    '#maxlength' => 60,
    '#default_value' => ( $discount_rule ? $discount_rule[ 'discount' ] : '' ),
    '#weight' => 10,
    '#description' => t(
    	'Enter a percent discount ( without the % sign ). So, for example, if you'
    	. ' want to have a 20% discount on the product price for this quantity'
    	. ' tier, then you would specify, 20.'
    ),
    '#required' => TRUE,
    '#element_validate' => array( 'uc_bulk_discount_validate_integer' ),
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#validate' => array( 'uc_bulk_discount_rule_form_validate' ),
    '#submit' => array( 'uc_bulk_discount_rule_form_submit' ),
    '#value' => t( 'Save rule' ),
    '#weight' => 12,
  );
  
  if ( $discount_rule ) {
    $form[ 'delete' ] = array(
      '#type' => 'submit',
      '#submit' => array( 'uc_bulk_discount_rule_delete_submit' ),
      '#value'=> t( 'Delete rule' ),
      '#weight' => 14,
    );
  }
  
  $form = array_merge(
    $form,
    uc_bulk_discount_user_form( $discount_rule ),
    uc_bulk_discount_role_form( $discount_rule ) 
  );
  
  return $form;
}

//------------------------------------------------------------------------------

function uc_bulk_discount_user_form( $discount_rule ) {
  
  $form = array();
  
  // Form inputs
  // * username [ textfield / autocomplete ] - uid
  
  if ( $discount_rule 
      && $discount_rule[ 'type' ] == UCBD_TYPE_USER 
      && array_key_exists( 'xid', $discount_rule ) ) {
         
    $account = user_load( $discount_rule[ 'xid' ] );
  }
  
  $form['user_name'] = array(
    '#type' => 'textfield',
  	'#id' => 'rule_user',
    '#title' => t('User name'),
    '#maxlength' => 60,
    '#autocomplete_path' => 'user/autocomplete',
    '#default_value' => $account ? $account->name : '',
    '#weight' => -1,
  );
  
  return $form;  
}

//------------------------------------------------------------------------------

function uc_bulk_discount_role_form( $discount_rule ) {
  
  $form = array();
  
  // Form inputs
  // * role name [ select box ] - rid
  
  if ( $discount_rule 
      && $discount_rule[ 'type' ] != UCBD_TYPE_ROLE
      && array_key_exists( 'xid', $discount_rule ) ) {
         
    $rid = $discount_rule[ 'xid' ];
  }
  
  $options = array();
  foreach ( user_roles() as $rid => $name ) {
    $options[ $rid ] = $name;
  }
  
  $form['user_role'] = array(
  	'#type' => 'select',
    '#id' => 'rule_role',
  	'#title' => t('User role'),
  	'#default_value' => ( $rid ? $rid : NULL ),
  	'#options' => $options,
  );
  
  return $form;    
}

//------------------------------------------------------------------------------

function uc_bulk_discount_validate_integer( $element, &$form_state ) {
	
	$title = strtolower( $element[ '#title' ] );
	$value = $element[ '#value' ];
	
	if ( ! is_numeric( $value ) || $value < 1 ) {
		form_error( $element, t( 'The @field field must be a positive integer.',
  														array( '@field' => $title ) ) );
	}
}

//------------------------------------------------------------------------------

function uc_bulk_discount_rule_form_validate( $form, &$form_state ) {
  $node          = $form[ '#node' ];
  $discount_rule = $form[ '#discount_rule' ];
  
  if ( $form_state[ 'values' ][ 'rule_type' ] ) {
    // Role based rule.
    $type = UCBD_TYPE_ROLE;
    $xid  = $form_state[ 'values' ][ 'user_role' ];
  }
  else {
    // User based rule.
    $type = UCBD_TYPE_USER;
    $xid  = $form_state[ 'values' ][ 'user_name' ];

    if ( !$xid || !( $account = user_load( array( 'name' => $xid ) ) ) ) {
      form_set_error( 'user_name', 
        t( 'The username %name does not exist.', array('%name' => $xid ) ) );
    }
    
    $xid = $account->uid;
  }
    
  // If adding new price rule, make sure it doesn't exist yet.
  if ( is_null( $discount_rule ) ) {
    
    $existing_rules = uc_bulk_discount_select_rules( 
      array( 
      	'nid' => $node->nid,
        'type' => $type,
        'xid'  => $xid 
      ),
      TRUE
    );
    
    // TODO: Throw in proper validation here.
    
    if ( count( $existing_rules ) ) {
      form_set_error( '', 
        t( 'This rule already exists.'
        	 . '  Go to the !edit_link to edit.',
        	array( '!edit_link' => l( t( 'edit page' ), 
        			"node/$node->nid/edit/discounts/edit/" . $existing_rule[ 'pdid' ] 
        	  ) 
        	) 
        ) 
      ); 
    }
  }
  else {
    // Adjust form values for submit ( to save some processing time ).
    $form_state[ 'values' ][ 'pdid' ] = $discount_rule[ 'pdid' ];
  }
  
  // Adjust form values for submit ( to save some processing time ).
  $form_state[ 'values' ][ 'nid' ]       = $node->nid;
  $form_state[ 'values' ][ 'rule_type' ] = $type;
  $form_state[ 'values' ][ 'xid' ]       = $xid;
}

//------------------------------------------------------------------------------

function uc_bulk_discount_rule_form_submit( $form, &$form_state ) {
  
  $nid = $form_state[ 'values' ][ 'nid' ];
  
  uc_bulk_discount_save_rule( array(
    'pdid'     => $form_state[ 'values' ][ 'pdid' ],
    'nid'      => $nid,
    'type'     => $form_state[ 'values' ][ 'rule_type' ],
    'xid'      => $form_state[ 'values' ][ 'xid' ],
    'min_qty'  => $form_state[ 'values' ][ 'min_qty' ],
  	'max_qty'  => $form_state[ 'values' ][ 'min_qty' ],
  	'discount' => $form_state[ 'values' ][ 'discount' ],
  ) );
  
  $form_state[ 'redirect' ] = "node/$nid/edit/discounts";
}

//------------------------------------------------------------------------------

function uc_bulk_discount_rule_delete_submit( $form, &$form_state ) {
  $node          = $form[ '#node' ];
  $discount_rule = $form[ '#discount_rule' ];
  
  $form_state[ 'redirect' ] = "node/$node->nid/edit/discounts/delete/" 
                            . $discount_rule[ 'pdid' ];
}

//------------------------------------------------------------------------------

function uc_bulk_discount_rule_delete_confirm_submit( $form, &$form_state ) {
  
  $nid = $form_state[ 'values' ][ 'nid' ];
  
  if ( $form_state['values']['confirm'] ) {
    uc_bulk_discount_remove_rule( $form_state[ 'values' ][ 'pdid' ] );  
  }
  
  $form_state[ 'redirect' ] = "node/$nid/edit/discounts";
}

//------------------------------------------------------------------------------
// Theme functions
//------------------------------------------------------------------------------

function theme_uc_bulk_discount_rule_listing( $node, $discount_rules ) {
  
  $header = array( t( 'Type' ), t( 'Applies to' ), 
  								 t( 'Low Quantity' ), t( 'High Quantity' ),  
  								 t( 'Discount Pecentage' ), '' );

	$rows   = array( );
  
  foreach ( $discount_rules as $rule ) {
    $row = array( );
    
    switch ( $rule[ 'type' ] ) {
      
      case UCBD_TYPE_USER:
        $type = t( 'User rule' );
        
        $account  = user_load( array( 'uid' => $rule[ 'xid'] ) );
        $for_name = $account->name;        
        break;
        
      case UCBD_TYPE_ROLE:
        $type = t( 'Role based rule' );
        
        $roles = user_roles( );
        $for_name = $roles[ $rule[ 'xid' ] ];
        break;
    }
    
    $row[ ] = $type;               
    $row[ ] = $for_name;
    $row[ ] = $rule[ 'min_qty' ];
    $row[ ] = $rule[ 'max_qty' ];
    $row[ ] = $rule[ 'discount' ] . '%';
    
    $row[ ] = l( t( 'edit' ), 
    	"node/$node->nid/edit/discounts/edit/" . $rule[ 'pdid' ], 
      array( 'class' => 'edit-rule-link' ) 
    ) . ' | '
    . l( t( 'delete' ),
      "node/$node->nid/edit/discounts/delete/" . $rule[ 'pdid' ],
      array( 'class' => 'delete-rule-link' )
    );
    
    $rows[ ] = $row;
  }
  
  $output = theme('table', $header, $rows);
  
  $output .= l( t( 'Add new discount rule' ), 
  	"node/$node->nid/edit/discounts/add", 
    array( 'class' => 'add-rule-link') 
  );
  
  return $output;
}

//------------------------------------------------------------------------------