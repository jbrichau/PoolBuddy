<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->
    <title>Poolbuddy</title>
    <!-- Bootstrap -->
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css" integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous">
  </head>
  <body>
    <div id="root"/>
    <form action="https://api.particle.io/v1/products/4513/customers" method="post">
      <input type="hidden" name="response_type" value="token" />
      <input type="hidden" name="client_id" value="poolbuddy-test-customer-331" />
      <div class="form-group">
        <input type="email" class="form-control" name="email" placeholder="Email">
      </div>
      <div class="form-group">
        <input type="password" class="form-control" name="password" placeholder="Password">
      </div>
      <input type="submit" class="btn btn-lg btn-primary btn-block" value="Register" id="registration-submit">
    </form>
  </body>
</html>
