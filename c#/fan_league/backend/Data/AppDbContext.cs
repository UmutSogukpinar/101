using fan_league.Entities;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata.Builders;

namespace fan_league.Data;

public class AppDbContext(DbContextOptions<AppDbContext> options)
    : DbContext(options)
{
    public DbSet<User> Users => Set<User>();
    public DbSet<NbaPlayer> NbaPlayers => Set<NbaPlayer>();
    public DbSet<NbaPlayerSeasonStat> NbaPlayerSeasonStats =>
        Set<NbaPlayerSeasonStat>();

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        EntityTypeBuilder<User> user = modelBuilder.Entity<User>();

        user.HasKey(x => x.Id);
        user.HasIndex(x => x.Username).IsUnique();
        user.HasIndex(x => x.Email).IsUnique();
        user.Property(x => x.Username).HasMaxLength(50).IsRequired();
        user.Property(x => x.Email).HasMaxLength(255).IsRequired();
        user.Property(x => x.PasswordHash).IsRequired();

        EntityTypeBuilder<NbaPlayer> nbaPlayer = modelBuilder.Entity<NbaPlayer>();

        nbaPlayer.HasKey(x => x.Id);
        nbaPlayer.HasIndex(x => x.NbaId).IsUnique();
        nbaPlayer.Property(x => x.FirstName).HasMaxLength(100).IsRequired();
        nbaPlayer.Property(x => x.LastName).HasMaxLength(100).IsRequired();
        nbaPlayer.Property(x => x.Team).HasMaxLength(100);
        nbaPlayer.Property(x => x.Position).HasMaxLength(20);
        nbaPlayer.Property(x => x.WeightKg).HasPrecision(6, 2);

        EntityTypeBuilder<NbaPlayerSeasonStat> seasonStat =
            modelBuilder.Entity<NbaPlayerSeasonStat>();

        seasonStat.HasKey(x => x.Id);
        seasonStat
            .HasIndex(x => new { x.NbaPlayerId, x.Season })
            .IsUnique();
        seasonStat
            .HasOne(x => x.NbaPlayer)
            .WithMany(x => x.SeasonStats)
            .HasForeignKey(x => x.NbaPlayerId)
            .OnDelete(DeleteBehavior.Cascade);
        seasonStat.Property(x => x.MinutesPerGame).HasPrecision(5, 2);
        seasonStat.Property(x => x.PointsPerGame).HasPrecision(5, 2);
        seasonStat.Property(x => x.ReboundsPerGame).HasPrecision(5, 2);
        seasonStat.Property(x => x.AssistsPerGame).HasPrecision(5, 2);
        seasonStat.Property(x => x.StealsPerGame).HasPrecision(5, 2);
        seasonStat.Property(x => x.BlocksPerGame).HasPrecision(5, 2);
        seasonStat.Property(x => x.TurnoversPerGame).HasPrecision(5, 2);
        seasonStat.Property(x => x.FieldGoalPercentage).HasPrecision(5, 2);
        seasonStat.Property(x => x.ThreePointPercentage).HasPrecision(5, 2);
        seasonStat.Property(x => x.FreeThrowPercentage).HasPrecision(5, 2);
    }
}
