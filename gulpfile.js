var gulp = require('gulp')
  , es6transpiler = require('gulp-es6-transpiler');


gulp.task('default', function () {
    return gulp.src('app/*.js')
        .pipe(es6transpiler())
        .pipe(gulp.dest('dist'));
});
