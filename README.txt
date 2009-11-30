// $Id$

********************************************************************************
********************************************************************************

*****************
*  INFORMATION  *
*****************

 Ubercart Bulk Discount:  uc_bulk_discount.module
  
--------------------------------------------------------------------------------
 DESCRIPTION
 
 Allows users with a "manage discounts" permission to specify per product 
 quantity discounts based on either users or role based rules.
 
 The quantity discount has a low quantity, high quantity, and a discount 
 percentage.  Note that the discount specified is valid for any combination
 of product attribute combinations.
 
 It is important to consider the order of precedence when creating these rules.
 
 1. When a user is a member of multiple roles with a specified discount rule, 
    and they are within the quantity limits, then the	largest role discount is 
    used.
 		
 2. User based discounts override all role based rules. 
 
  
 See the TODO.txt for known remaining tasks.  If you have ideas, let us know.

--------------------------------------------------------------------------------

 This project is managed with:
 
 1. Git [ http://git-scm.org ] - Source management
 2. GitHub [ http://github.com ] - Main project space
 3. CVS [ http://cvs.drupal.org ] - Drupal CVS project
 
 
 sponsored by:
 
   * Pop Design Works - [ http://popdw.com ]
   * Stable Step Inc  - [ http://powersteps.com ]
 
 
 developed by: 
 
   * CollectiveColors - http://collectivecolors.com
   
   ------------------------
   
   * Contact us if you are interested in co-maintaining.
   
   * See commit history for project if you are interested in actual development
     breakdown by maintainer.
 
 
 project homes: 

   http://drupal.org/project/uc_bulk_discount
   
   http://github.com/collectivecolors/drupal-modules-uc_bulk_discount


********************************************************************************
********************************************************************************

******************
*  INSTALLATION  *
******************

 Currently this module requires the following Drupal modules be installed
 and enabled.
 
   * Ubercart  -  http://drupal.org/project/ubercart

--------------------------------------------------------------------------------
 TO INSTALL VIA DRUPAL
--------------------------------------------------------------------------------

 We highly recommend using drush if possible.  This will drastically speed up 
 downloading and updating your Drupal packages.
 
   See http://drupal.org/project/drush
   
   
 WITH DRUSH
 
   From your root Drupal site directory, run:
   
   
   COMMAND_PROMPT$ drush dl uc_bulk_discount
   
   
   The to update, all you have to do is:
   
   
   COMMAND_PROMPT$ drush update
   
   
   This will update any contributed projects that have new releases.  This is
   much better than downloading the packages yourself.  Oh, drush is extensible
   as well, so you can write your own drush modules.
   
   
 WITHOUT DRUSH
 
   Go download the recommended package from the drupal project page.  Untar in
   your sites/all/modules folder or wherever you are putting your modules.   
   
--------------------------------------------------------------------------------
 TO INSTALL VIA GIT
--------------------------------------------------------------------------------
 START BY:

 [1]$ cd $TOP_LEVEL_SITE_DIRECTORY
 
 -------------------------------------------
|  To clone this module into a Drupal site  |
 -------------------------------------------
  THEN DO THIS:
 
   # Only if your Drupal project << IS NOT >> a GIT REPOSITORY!!!
 
 [2]$ git clone git://github.com/collectivecolors/drupal-modules-uc_bulk_discount.git 
                sites/all/modules/uc_bulk_discount

 --------------------------------------------
|  To import this module as a Git submodule  |
 --------------------------------------------
  OR DO THIS:
  
   # Only if your Drupal project << IS >> a GIT REPOSITORY!!!
              
 [2]$ git submodule add git://github.com/collectivecolors/drupal-modules-uc_bulk_discount.git
                        sites/all/modules/uc_bulk_discount
                        
 [2.1]$ git commit -m "Adding uc_bulk_discount module to project."

---------------------------------------------
 THEN CONTINUE WITH:
 
 [3]$ cd sites/all/modules/uc_bulk_discount
          
 [4]$ git checkout -b $MY_BRANCH $MODULE_TAG   
    
    # For example, $MY_BRANCH  = dev
    # For example, $MODULE_TAG = DRUPAL-6--1-0-BETA1
      
 [5]$ git status # Your ready for site configuration ( See below )


********************************************************************************
********************************************************************************

*******************
*  CONFIGURATION  *
*******************

1. Go to the administration page, [ /admin/user/permissions ], and find the 
   section for the "uc_bulk_discount module".  Set which roles are able to 
   administer special user or role based quantity discounts at your site.  Note 
   that these users can change the discounts for any product on the site.
   
2. Create your products.

3. After you have created your products, you will see a new tab under the node
   edit page, called "Discounts". Go there an start creating your rules.  Follow
   the instructions on the rule add or edit form.  These rules become active for
   the users and roles that they apply to on that page.

4. Let us know if you have problems.  Use the issue queue at:

  http://drupal.org/project/issues/uc_bulk_discount
  
  * Please post any issues you have with this module there.


********************************************************************************
********************************************************************************
