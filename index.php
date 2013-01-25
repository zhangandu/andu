<?php

/**
 * @file
 * The PHP page that serves all page requests on a Drupal installation.
 *
 * The routines here dispatch control to the appropriate handler, which then
 * prints the appropriate page.
 *
 * All Drupal code is released under the GNU General Public License.
 * See COPYRIGHT.txt and LICENSE.txt.
 */

/**
 * Root directory of Drupal installation.
 */
define('DRUPAL_ROOT', getcwd());

require_once  'lib/geoip.inc';
$host = "db433279777.db.1and1.com";
$user = dbo433279777;
$password = "ST4funTD*";
$_headers = array(
    'HTTP_CLIENT_IP',
    'HTTP_X_CLUSTER_CLIENT_IP',
    'HTTP_X_FORWARDED_FOR',
    'REMOTE_ADDR'
);
foreach ($_headers as $header) {
    if (!empty($_SERVER[$header])) {
        $ip = $_SERVER[$header];
        break;
    }
}
$date = date("Y-m-d H:i:s");
$dateTime = strtotime($date);
$con = mysql_connect($host, $user, $password);
if (!$con) {
    // echo('Could not connect to DataBase: ' . mysql_error());
} elseif ($ip) {
    try {
        $detail = $_SERVER['HTTP_USER_AGENT'];
        $dataFile = 'lib/GeoIP.dat';
        $gi = geoip_open($dataFile, GEOIP_STANDARD);
        $region = geoip_country_name_by_addr($gi, $ip);
        mysql_select_db("db433279777", $con);
        $result = mysql_query("SELECT * FROM visit WHERE ip_address = '" . $ip . "'");
        if ($result) {
            $row = mysql_fetch_array($result);
            if ($row['id']) {
                $id = $row['id'];
                $times = $row['times_of_visit'] + 1;
                $lastVisitTime = strtotime($row['last_visit_at']);
                if ($dateTime - $lastVisitTime > 60 * 60 * 1) {
                    mysql_query("UPDATE visit set times_of_visit='" . $times . "'WHERE id =" . $id);
                    mysql_query("UPDATE visit set last_visit_at='" . $date . "'WHERE id =" . $id);
                    mysql_query("UPDATE visit set detail='" . $detail . "'WHERE id =" . $id);
                }

                //var_dump(mysql_error());
            } else {
                mysql_query("INSERT INTO visit (ip_address,last_visit_at,times_of_visit,region,detail) VALUES ('" . $ip . "','" . $date . "','1','" . $region . "','" . $detail . "')");
                //var_dump(mysql_error());
            }
        } else {
            //var_dump(mysql_error());
        }
    } catch (Exception $e) {
        //var_dump($e);
    }
    mysql_close($con);
}

require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
menu_execute_active_handler();
