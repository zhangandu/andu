<?php
if(isset($_REQUEST['contact']) && $_REQUEST['contact'] ){
    $contact = true;
}
$validate = true;
$post = array('name','email','reason','detail');
foreach($post as $index){
    if(!isset($_REQUEST[$index]) || empty($_REQUEST[$index])){
        $validate = false;
        break;
    }
}if($validate){
    $host = "db433279777.db.1and1.com";
    $user = dbo433279777;
    $password = "ST4funTD*";
    $name = $_REQUEST['name'];
    $email = $_REQUEST['email'];
    $reason = $_REQUEST['reason'];
    $detail = $_REQUEST['detail'];
    $con = mysql_connect($host, $user, $password);
    if (!$con) {
        //echo('Could not connect to DataBase: ' . mysql_error());
    }else{
        mysql_select_db("db433279777", $con);
        mysql_query("INSERT INTO message (name,email,reason,detail) VALUES ('" . $name . "','" . $email . "','".$reason."','" . $detail . "')");
        //echo mysql_error();
    }
}
?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xmlns="http://www.w3.org/1999/html" xmlns="http://www.w3.org/1999/html"
      xmlns="http://www.w3.org/1999/html"
      xml:lang="en-US" lang="en-US">
<head>
    <meta content="text/html; charset=utf-8" http-equiv="content-type">
    <title>Contact Zhang Andu</title>
    <link type="text/css" href="styles/style.css" rel="stylesheet">
    <link type="text/css" href="styles/home.css" rel="stylesheet">
    <link type="text/css" href="styles/jquery.lightbox-0.5.css" rel="stylesheet">
    <script src="//ajax.googleapis.com/ajax/libs/jquery/1.8.3/jquery.min.js">
    </script>
    <script src="js/jquery.lightbox-0.5.js">
    </script>
    <script type="text/javascript">
        var _gaq = _gaq || [];
        _gaq.push(['_setAccount', 'UA-37673900-1']);
        _gaq.push(['_trackPageview']);

        (function() {
            var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
            ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
            var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
        })();
    </script>
    <script>
        (function() {
            var cx = '002487588673075056338:knvs0wxeldk';
            var gcse = document.createElement('script'); gcse.type = 'text/javascript'; gcse.async = true;
            gcse.src = (document.location.protocol == 'https:' ? 'https:' : 'http:') +
                    '//www.google.com/cse/cse.js?cx=' + cx;
            var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(gcse, s);
        })();
    </script>
    <script type="text/javascript">
        $(function() {
            $('#image-gallery a').lightBox();
        });
    </script>
</head>
<body>
<div id="page">
    <div class="background"></div>
    <div id="header">
        <a id="logo" rel="home" title="Home" href="/">
            <img alt="Home" src="images/logo.jpg"></a>
        <audio controls autoplay="autoplay" loop="loop">
            <source src="videos/waimiandeshijie.wav" type="audio/wav">
            Your browser does not support the audio element.
        </audio>
        <!-- Place this tag where you want the search box to render -->
        <gcse:searchbox-only></gcse:searchbox-only>
        <div id="nav">
            <ul>
                <li><a href="index.php">Portfolio</a></li>
                <li><a href="blog">Blog</a></li>
                <li><a href="forum">Forum</a></li>
                <li><a href="store">Store</a></li>
                <li><a href="contact.php">Contact</a></li>
            </ul>
        </div>
    </div>
    <div id="main">
        <div id ="contact">
            <h2> Welcome to inquire about projects including personal, blog, forum, e-commerce website.</h2>
            <form method="post" action="contact.php">
                <input type="hidden" name="contact" value=1 />
                <span>Name: </span><input name="name" type="text" /><br>
                <span>Email: </span><input name="email" type="email" /><br>
                <span>Title: </span><input name="reason" type="text"/><note>Say hi, help for projects etc.</note><br>
                <span>Detail: </span><textarea NAME="detail" ROWS=5 COLS=30 ></textarea>
            </TEXTAREA>
                <button type="submit"></button>
            </form>
            <?php if($contact):?>
            <div id="contact-post-result">
                <?php if($validate): ?>
                <p>Your message is successfully submitted!</p>
                <?php else: ?>
                <p>Please fill all fields above!</p>
                <?php endif; ?>
            </div>
            <?php endif; ?>
        </div>
    </div>
    <div id="footer">
        <span>Zhang Andu, Email: azhang@zhangandu.info</span>
    </div>
</div>
</body>
</html>